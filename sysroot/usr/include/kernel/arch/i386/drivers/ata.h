#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>


/* Number of connected ATA devices. */
/* The device ID passed to the write and read functions must be lower than this. */
unsigned char num_ata_devices;


/**
 * Initializes the ATA driver.
 * 
 * @return the number of connected ATA devices
*/
unsigned char ata_init(void);


/**
 * Writes to an ATA device.
 * 
 * The offset and size arguments must be multiples of the disk's logical sector size.
 * 
 * @param dev_id the device ID
 * @param data a pointer to write the data from
 * @param lba the lba to start writing the data to
 * @param sector_count the number of sectors to write
 * 
 * @return true if the write was successful, false otherwise
*/
bool ata_write(unsigned char dev_id, void* data, uint64_t lba, uint16_t sector_count);

/**
 * Reads from an ATA device.
 * 
 * The offset and size arguments must be multiples of the disk's logical sector size.
 * 
 * @param dev_id the device ID
 * @param buf a pointer to where to store the data
 * @param lba the lba to start writing the data to
 * @param sector_count the number of sectors to write
 * 
 * @return true if the read was successful, false otherwise
*/
bool ata_read(unsigned char dev_id, void* buf, uint64_t lba, uint16_t sector_count);
