#ifndef _KERNEL_MEMORY_H
#define _KERNEL_MEMORY_H

#include <kernel/multiboot.h>
#include <stdint.h>
#include <stddef.h>

extern char _kernel_offset;

#define KERNEL_OFFSET 	((unsigned long) &_kernel_offset)

#define P2V(p)			(p + KERNEL_OFFSET)
#define V2P(v)			(v - KERNEL_OFFSET)

/**
 * Detects the amount of memory available in the system and initializes
 * the fields required for memory allocation.
 * 
 * @param mbi the multiboot info structure
*/
void memory_detect(multiboot_info_t* mbi);


/**
 * Allocates the specified number of memory blocks and writes their physical memory addresses in the given array, marking them as used.
 * 
 * @param addrs the array to store the physical memory addresses of the allocated blocks
 * @param count the number of blocks to allocate
 * 
 * @return the number of successively allocated blocks
*/
size_t mblocks_alloc(unsigned long* addrs, size_t count);

/**
 * Frees up the specified memory blocks, marking them as free.
 * 
 * @param addrs the array containing the phyical memory addresses of the memory to be freed
 * @param count the number of blocks to free
*/
void mblocks_free(const unsigned long* addrs, size_t count);


#endif
