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
 * @param offset the offset to where to write the data to
 * @param size the size of the data to be written
*/
void ata_write(unsigned char dev_id, void* data, uintptr_t offset, size_t size);

/**
 * Reads from an ATA device.
 * 
 * The offset and size arguments must be multiples of the disk's logical sector size.
 * 
 * @param dev_id the device ID
 * @param buf a pointer to where to store the data
 * @param offset the offset to where the data is located
 * @param size the size of the data to be read
*/
void ata_read(unsigned char dev_id, void* buf, uintptr_t offset, size_t size);
