/**
 * Code for the ATA driver.
 * 
 * TODO: detect and fix temporary bad sectors, support reads and writes with size < logical sector's
 * 
 * Refer to:
 * https://wiki.osdev.org/ATA_PIO_Mode
 * 
 * @author Samuel Pires
*/

#include <kernel/arch/i386/drivers/ata.h>

#include <kernel/system.h>
#include <kernel/arch/i386/io.h>

#include <stdio.h>
#include <string.h>


#define PRIMARY_PORT_BASE 		0x1F0
#define SECONDARY_PORT_BASE 	0x170

#define PRIMARY_IRQ				0x14
#define ECONDARY_IRQ			0x15

#define IDENTIFY_NUM_WORDS		256

#define SECTOR_DEFAULT_SIZE		512

#define PORT_DATA				0x0
#define PORT_ERROR				0x1
#define PORT_FEATURES			0x1
#define PORT_SECTOR_COUNT		0x2
#define PORT_LBA_LOW			0x3
#define PORT_LBA_MID			0x4
#define PORT_LBA_HI				0x5
#define PORT_DEVICE_SELECT		0x6
#define PORT_COMMAND			0x7
#define PORT_STATUS				0x7
#define PORT_CONTROL			0x206

#define COMMAND_WRITE_SECTORS		0x30
#define COMMAND_READ_SECTORS		0x20
#define COMMAND_READ_SECTORS_EXT	0x24
#define COMMAND_WRITE_SECTORS_EXT	0x34
#define COMMAND_IDENTIFY			0xEC
#define COMMAND_FLUSH				0xE7

#define STATUS_ERR				1
#define STATUS_DRQ				(1 << 3)
#define STATUS_DF				(1 << 5)	/* Drive Fault Error */
#define STATUS_BSY				(1 << 7)

#define POLL_ERR_MASK			(STATUS_ERR | STATUS_DF)

#define LBA28_MAX_SECTOR_COUNT	256
#define LBA48_MAX_SECTOR_COUNT	65536


/* The ID of the currently selected device */
unsigned char selected_dev_id;


static bool ata_device_init(ata_device_t* dev, uint16_t port_base, bool master);
static bool ata_identify(ata_device_t* dev, uint16_t* buf);

static bool ata_read28(ata_device_t* dev, uint16_t* buf, uint32_t lba, uint8_t sector_count);
static bool ata_write28(ata_device_t* dev, uint16_t* data, uint32_t lba, uint8_t sector_count);
static bool ata_read48(ata_device_t* dev, uint16_t* buf, uint64_t lba, uint16_t sector_count);
static bool ata_write48(ata_device_t* dev, uint16_t* data, uint64_t lba, uint16_t sector_count);

static void ata_pio28_prepare(ata_device_t* dev, uint32_t lba, uint8_t sector_count);
static void ata_pio48_prepare(ata_device_t* dev, uint64_t lba, uint16_t sector_count);
static bool ata_read_transfer(ata_device_t* dev, uint16_t* buf, uint32_t sector_count);
static bool ata_write_transfer(ata_device_t* dev, uint16_t* data, uint32_t sector_count);

static void ata_select(ata_device_t* dev);
static void ata_io_wait(ata_device_t* dev);
static bool ata_poll(ata_device_t* dev);


/* Global Functions */

unsigned char ata_init(void)
{
	ASSERT(num_ata_devices == 0);

	num_ata_devices += ata_device_init(ata_devices, PRIMARY_PORT_BASE, true);
	num_ata_devices += ata_device_init(ata_devices + num_ata_devices, PRIMARY_PORT_BASE, false);
	num_ata_devices += ata_device_init(ata_devices + num_ata_devices, SECONDARY_PORT_BASE, true);
	num_ata_devices += ata_device_init(ata_devices + num_ata_devices, SECONDARY_PORT_BASE, false);

	ata_select(ata_devices);
	selected_dev_id = 0;

	return num_ata_devices;
}


bool ata_read(unsigned char dev_id, void* buf, uint64_t offset, uint64_t size)
{
	ASSERT(dev_id < num_ata_devices);

	if (size == 0)
		return true;

	if (offset + size > ATA_DISK_SIZE(dev_id))
		return false;

	ata_device_t* dev = ata_devices + dev_id;

	/* Divide read operation if size exceeds the maximum possible for a single one */
	if (size / dev->logical_sector_size > LBA48_MAX_SECTOR_COUNT) {
		uint32_t op_max_size = LBA48_MAX_SECTOR_COUNT * dev->logical_sector_size;
		return ata_read(dev_id, buf, offset, op_max_size) &&
			   ata_read(dev_id, buf + op_max_size, offset, size - op_max_size);
	}

	uint64_t lba = offset / dev->logical_sector_size;
	uint16_t sector_count = size / dev->logical_sector_size;

	if (dev_id != selected_dev_id)
		ata_select(dev);

	if (lba < dev->lba28_num_sectors && !(sector_count & 0xFF00))
		return ata_read28(dev, buf, lba, sector_count);
	if (lba < dev->lba48_num_sectors)
		return ata_read48(dev, buf, lba, sector_count);
	
	return false;
}

bool ata_write(unsigned char dev_id, void* data, uint64_t offset, uint64_t size)
{
	ASSERT(dev_id < num_ata_devices);

	if (size == 0)
		return true;

	if (offset + size > ATA_DISK_SIZE(dev_id))
		return false;

	ata_device_t* dev = ata_devices + dev_id;

	/* Restrict writes in sector 0 */
	uint64_t lba = offset / dev->logical_sector_size;
	if (lba == 0)
		return false;

	/* Divide write operation if size exceeds the maximum possible for a single one */
	if (size / dev->logical_sector_size > LBA48_MAX_SECTOR_COUNT) {
		uint32_t op_max_size = LBA48_MAX_SECTOR_COUNT * dev->logical_sector_size;
		return ata_write(dev_id, data, offset, op_max_size) &&
			   ata_write(dev_id, data + op_max_size, offset, size - op_max_size);
	}

	uint16_t sector_count = size / dev->logical_sector_size;

	if (dev_id != selected_dev_id)
		ata_select(dev);

	if (lba < dev->lba28_num_sectors && !(sector_count & 0xFF00))
		return ata_write28(dev, data, lba, sector_count);
	if (lba < dev->lba48_num_sectors)
		return ata_write48(dev, data, lba, sector_count);
	
	return false;
}


/* Helper Functions */

/**
 * Initializes an ATA dev.
 * 
 * @param dev the device to initialize
 * @param port_base the base IO port that connects to the disk controller
 * @param master whether the disk is master or not (slave)
 * 
 * @return true if the dev is connected, false otherwise
*/
static bool ata_device_init(ata_device_t* dev, uint16_t port_base, bool master)
{
	dev->port_base = port_base;
	dev->master = master;

	uint16_t buf[IDENTIFY_NUM_WORDS];

	if (!ata_identify(dev, buf))
		return false;

	/* Words 60 & 61 taken as a uint32_t contain the total number of 28 bit LBA addressable sectors on the drive */
	dev->lba28_num_sectors = (buf[61] << 16) | buf[60];

	/* Word 83: Bit 10 is set if the drive supports LBA48 mode */
	if (buf[83] & (1 << 10)) {
		/*  Words 100 through 103 taken as a uint64_t contain the total number of 48 bit addressable sectors on the drive */
		dev->lba48_num_sectors = (((uint64_t) buf[103]) << 48) |
								 (((uint64_t) buf[102]) << 32) |
								 (((uint32_t) buf[101]) << 16) |
								 buf[100];
	}
	else {
		dev->lba48_num_sectors = 0;
	}

	/* Word 88 */
	/* The lower bytes tells which UDMA modes are supported */
	dev->supported_udma_modes = buf[88] & 0xFF;
	/* The upper byte tells which UDMA mode is active */
	dev->active_udma_mode = buf[88] >> 8;

	dev->logical_sector_size = SECTOR_DEFAULT_SIZE;
	dev->physical_sector_size = SECTOR_DEFAULT_SIZE;
	dev->logical_sector_alignment = 0;

	/* Retrieve info about drive's logical and physical sectors */
	if (!(buf[106] & (1 << 15)) && buf[106] & (1 << 14))
	{
		if (buf[106] & (1 << 12))
			dev->logical_sector_size = ((buf[118] << 16) | buf[117]) * sizeof(uint16_t);

		dev->physical_sector_size = dev->logical_sector_size;

		if (buf[106] & (1 << 13)) {
			dev->physical_sector_size = dev->logical_sector_size << (buf[106] & 0x0F);

			if (!(buf[209] & (1 << 15)) && buf[209] & (1 << 14))
				dev->logical_sector_alignment = buf[209] & 0xC;
		}
	}

	return true;
}

/**
 * Runs the ATA IDENTIFY command on a dev.
 * 
 * @param dev the device to run the command on
 * @param buf a buffer to store the data returned by the IDENTIFY command
 * 
 * @return true if the dev is connected, is ATA, and no errors ocurred; false otherwise
*/
static bool ata_identify(ata_device_t* dev, uint16_t* buf)
{
	ata_select(dev);

	/* Set the Sectorcount, LBAlo, LBAmid, and LBAhi IO ports to 0 */
	outb(dev->port_base + PORT_SECTOR_COUNT, 0);
	outb(dev->port_base + PORT_LBA_LOW, 0);
	outb(dev->port_base + PORT_LBA_MID, 0);
	outb(dev->port_base + PORT_LBA_HI, 0);
	
	/* Send the IDENTIFY command to the Command IO port */
	outb(dev->port_base + PORT_COMMAND, COMMAND_IDENTIFY);

	/* Read the Status port. If the value read is 0, the drive does not exist */
	if (!inb(dev->port_base + PORT_STATUS))
		return false;

	uint8_t status;

	/* Poll the Status port until BSY clears */
	unsigned int timer = 0xFFFFFF;
	while (--timer) {
		status = inb(dev->port_base + PORT_STATUS);

		if (!(status & STATUS_BSY))
			break;

		if (status & STATUS_ERR)
			return false;
	}

	if (timer == 0)
		return false;

	/* Check the LBAmid and LBAhi ports to see if they are non-zero. If so, the drive is not ATA */
	if (inb(dev->port_base + PORT_LBA_MID) || inb(dev->port_base + PORT_LBA_HI))
		return false;

	/* Poll the Status port until DRQ sets */
	timer = 0xFFFFFF;
	while (--timer) {
		status = inb(dev->port_base + PORT_STATUS);

		if (status & STATUS_DRQ)
			break;

		if (status & STATUS_ERR)
			return false;
	}

	if (timer == 0)
		return false;

	/* Read the data from the IDENTIFY command */
	for (int i = 0; i < IDENTIFY_NUM_WORDS; i++)
		buf[i] = inw(dev->port_base + PORT_DATA);

	return true;
}


/**
 * Reads data from an ATA device using 28-bit PIO.
 * 
 * Assumes that the device is already selected.
 * 
 * @param dev the device
 * @param buf the buffer to read the data to
 * @param lba the LBA
 * @param sector_count the number of sectors to read
 * 
 * @return true if the read completed successfully, false otherwise
*/
static bool ata_read28(ata_device_t* dev, uint16_t* buf, uint32_t lba, uint8_t sector_count)
{
	ata_pio28_prepare(dev, lba, sector_count);
	outb(dev->port_base + PORT_COMMAND, COMMAND_READ_SECTORS);
	return ata_read_transfer(dev, buf, sector_count ? sector_count : LBA28_MAX_SECTOR_COUNT);
}

/**
 * Writes data to an ATA device using 28-bit PIO.
 * 
 * Assumes that the device is already selected.
 * 
 * @param dev the device
 * @param data an array to write the data from
 * @param lba the LBA
 * @param sector_count the number of sectors to write
 * 
 * @return true if the write completed successfully, false otherwise
*/
static bool ata_write28(ata_device_t* dev, uint16_t* data, uint32_t lba, uint8_t sector_count)
{
	ata_pio28_prepare(dev, lba, sector_count);
	outb(dev->port_base + PORT_COMMAND, COMMAND_WRITE_SECTORS);
	return ata_write_transfer(dev, data, sector_count ? sector_count : LBA28_MAX_SECTOR_COUNT);
}

/**
 * Reads data from an ATA device using 48-bit PIO.
 * 
 * Assumes that the device is already selected.
 * 
 * @param dev the device
 * @param buf the buffer to read the data to
 * @param lba the LBA
 * @param sector_count the number of sectors to read
 * 
 * @return true if the read completed successfully, false otherwise
*/
static bool ata_read48(ata_device_t* dev, uint16_t* buf, uint64_t lba, uint16_t sector_count)
{
	ata_pio48_prepare(dev, lba, sector_count);
	outb(dev->port_base + PORT_COMMAND, COMMAND_READ_SECTORS_EXT);
	return ata_read_transfer(dev, buf, sector_count ? sector_count : LBA48_MAX_SECTOR_COUNT);
}

/**
 * Writes data to an ATA device using 48-bit PIO.
 * 
 * Assumes that the device is already selected.
 * 
 * @param dev the device
 * @param data an array to write the data from
 * @param lba the LBA
 * @param sector_count the number of sectors to write
 * 
 * @return true if the write completed successfully, false otherwise
*/
static bool ata_write48(ata_device_t* dev, uint16_t* data, uint64_t lba, uint16_t sector_count)
{
	ata_pio48_prepare(dev, lba, sector_count);
	outb(dev->port_base + PORT_COMMAND, COMMAND_WRITE_SECTORS_EXT);
	return ata_write_transfer(dev, data, sector_count ? sector_count : LBA48_MAX_SECTOR_COUNT);
}


/**
 * Prepares a device for a PIO LBA28 operation.
 * 
 * @param dev the device to prepare
 * @param lba the operation's starting LBA
 * @param sector_count the operation's sector count
*/
static void ata_pio28_prepare(ata_device_t* dev, uint32_t lba, uint8_t sector_count)
{
	/* Assert that the lba is addressable with 28 bits */
	ASSERT(!(lba & 0xF0000000));

	/* Send 0xE0 for the "master" or 0xF0 for the "slave", ORed with the highest 4 bits of the LBA to the drive select port */
	outb(dev->port_base + PORT_DEVICE_SELECT, (dev->master ? 0xE0 : 0xF0) | (lba >> 24));

	/* Send the sectorcount to the sectorcount port */
	outb(dev->port_base + PORT_SECTOR_COUNT, sector_count);

	/* Send the LBA to the LBA ports */
	outb(dev->port_base + PORT_LBA_LOW, lba);
	outb(dev->port_base + PORT_LBA_MID, lba >> 8);
	outb(dev->port_base + PORT_LBA_HI, lba >> 16);
}

/**
 * Prepares a device for a PIO LBA48 operation.
 * 
 * @param dev the device to prepare
 * @param lba the operation's starting LBA
 * @param sector_count the operation's sector count
*/
static void ata_pio48_prepare(ata_device_t* dev, uint64_t lba, uint16_t sector_count)
{
	/* Assert that the lba is addressable with 48 bits */
	ASSERT(!(lba & 0xFFFF000000000000));

	outb(dev->port_base + PORT_DEVICE_SELECT, dev->master ? 0x40 : 0x50);
	outb(dev->port_base + PORT_SECTOR_COUNT, sector_count >> 8);
	outb(dev->port_base + PORT_LBA_LOW, lba >> 24);
	outb(dev->port_base + PORT_LBA_MID, lba >> 32);
	outb(dev->port_base + PORT_LBA_HI, lba >> 40);
	outb(dev->port_base + PORT_SECTOR_COUNT, sector_count);
	outb(dev->port_base + PORT_LBA_LOW, lba);
	outb(dev->port_base + PORT_LBA_MID, lba >> 8);
	outb(dev->port_base + PORT_LBA_HI, lba >> 16);
}

/**
 * Transfers data between a device and memory after a read command has been sent.
 * 
 * The sector_count argument must correspond to the actual number of sectors to read, not zero.
 * 
 * @param dev the device read from
 * @param buf the buffer to write to
 * @param sector_count the number of sectors to read
 * 
 * @return true if the data transfer was successful, false otherwise
*/
static bool ata_read_transfer(ata_device_t* dev, uint16_t* buf, uint32_t sector_count)
{
	uint32_t words_per_sector = dev->logical_sector_size / sizeof(uint16_t);

	for (uint32_t i = 0; i < sector_count; i++)
	{
		/* Wait until device is ready */
		if (!ata_poll(dev))
			return false;

		/* Transfer 256 16-bit values, a word at a time, from the data port into your buffer */
		for (uint32_t j = 0; j < words_per_sector; j++)
			buf[i * words_per_sector + j] = inw(dev->port_base + PORT_DATA);

		ata_io_wait(dev);
	}

	return true;
}

/**
 * Transfers data between memory and a device after a write command has been sent.
 * 
 * The sector_count argument must correspond to the actual number of sectors to write, not zero.
 * 
 * @param dev the device write to
 * @param data the address to read from
 * @param sector_count the number of sectors to write
 * 
 * @return true if the data transfer was successful, false otherwise
*/
static bool ata_write_transfer(ata_device_t* dev, uint16_t* data, uint32_t sector_count)
{
	uint32_t words_per_sector = dev->logical_sector_size / sizeof(uint16_t);

	for (uint32_t i = 0; i < sector_count; i++)
	{
		/* Wait until device is ready*/
		if (!ata_poll(dev))
			return false;

		/* Transfer 256 16-bit values, a word at a time, from memory into the data port */
		for (uint32_t j = 0; j < words_per_sector; j++)
			outw(dev->port_base + PORT_DATA, data[i * words_per_sector + j]);

		/* Flush the data */
		outb(dev->port_base + PORT_COMMAND, COMMAND_FLUSH);
		while (inb(dev->port_base + PORT_STATUS) & STATUS_BSY) {}
	}

	return true;
}



/**
 * Selects a drive to be used.
 * 
 * @param dev the device to be selected
*/
static void ata_select(ata_device_t* dev)
{
	/* Select a target drive by sending 0xA0 for the master drive, or 0xB0 for the slave, to the "drive select" IO port */
	outb(dev->port_base + PORT_DEVICE_SELECT, dev->master ? 0xA0 : 0xB0);
	ata_io_wait(dev);
}

/**
 * Polls an ATA device until it becomes ready.
 * 
 * Assumes that the device is already selected.
 * 
 * @param dev the device
 * 
 * @return true if the device is ready, false if an error ocurred
*/
static bool ata_poll(ata_device_t* dev)
{
	/* Read the Regular Status port until the BSY bit clears, and the DRQ bit sets or until the ERR bit or DF bit sets.
	   If neither error bit is set, the device is ready. */
	int timer = 0xFFFFFF;
	while (--timer) {
		uint8_t status = inb(dev->port_base + PORT_STATUS);

		if (!(status & STATUS_BSY) && (status & STATUS_DRQ))
			break;

		if (status & POLL_ERR_MASK)
			return false;
	}

	return timer > 0;
}

static void ata_io_wait(ata_device_t* dev)
{
	/* Wait 400 nanoseconds */
	inb(dev->port_base + PORT_CONTROL);
	inb(dev->port_base + PORT_CONTROL);
	inb(dev->port_base + PORT_CONTROL);
	inb(dev->port_base + PORT_CONTROL);
}
