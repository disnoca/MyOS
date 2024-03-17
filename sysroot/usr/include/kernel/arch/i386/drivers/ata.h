#pragma once

#include <kernel/utils.h>

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>


typedef struct ata_device_s {
	uint16_t port_base;
	bool master;

	uint32_t logical_sector_size;
	uint32_t physical_sector_size;
	uint16_t logical_sector_alignment;

	/* if sectors = 0, mode is not supported */
	uint32_t lba28_num_sectors;
	uint64_t lba48_num_sectors;

	uint8_t supported_udma_modes;	/* mode n is supported if bit n-1 is set */
	uint8_t active_udma_mode;		/* active UDMA mode */
} ata_device_t;


/* An array of connected ATA devices */
/* Only the first num_ata_devices devices are valid. */
ata_device_t ata_devices[4];

/* Number of connected ATA devices. */
/* The device ID passed to the write and read functions must be lower than this. */
unsigned char num_ata_devices;


#define ATA_DISK_SIZE(dev_id)	({ ata_device_t dev = ata_devices[dev_id]; MAX(dev.lba28_num_sectors, dev.lba48_num_sectors) * dev.logical_sector_size; })


/**
 * Initializes the ATA driver.
 * 
 * @return the number of connected ATA devices
*/
unsigned char ata_init(void);


/**
 * Writes to an ATA device.
 * 
 * @param dev_id the device ID
 * @param data a pointer to write the data from
 * @param offset the address at which to write
 * @param size the size of data to write
 * 
 * @return true if the write was successful, false otherwise
*/
bool ata_write(unsigned char dev_id, const void* data, uint64_t offset, uint64_t size);

/**
 * Reads from an ATA device.
 * 
 * @param dev_id the device ID
 * @param buf a pointer to where to store the data
 * @param offset the address at which to read
 * @param size the size of the data to read
 * 
 * @return true if the read was successful, false otherwise
*/
bool ata_read(unsigned char dev_id, void* buf, uint64_t offset, uint64_t size);
