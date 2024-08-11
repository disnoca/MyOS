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


#define ATA_NUM_SECTORS(dev_id)	({ ata_device_t dev = ata_devices[dev_id]; MAX(dev.lba28_num_sectors, dev.lba48_num_sectors); })


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
 * @param lba the lba to start writing the data to
 * @param sector_count the number of sectors to write
 * 
 * @return 0 if the write was successful, -1 otherwise
*/
int ata_write(unsigned char dev_id, const void* data, uint64_t lba, uint16_t sector_count);

/**
 * Reads from an ATA device.
 * 
 * @param dev_id the device ID
 * @param buf a pointer to where to store the data
 * @param lba the lba to start reading the data from
 * @param sector_count the number of sectors to read
 * 
 * @return 0 if the read was successful, -1 otherwise
*/
int ata_read(unsigned char dev_id, void* buf, uint64_t lba, uint16_t sector_count);
