#pragma once

#include <kernel/multiboot.h>
#include <kernel/arch/i386/paging.h>

#include <stdint.h>
#include <stddef.h>

extern char _kernel_offset;

#define KERNEL_OFFSET 			((unsigned long) &_kernel_offset)

#define P2V(p)					(p + KERNEL_OFFSET)
#define V2P(v)					(v - KERNEL_OFFSET)

/**
 * Initializes the memory manager detecting the amount of available memory in the system,
 * creating a memory bitmap and marking the detected available memory areas as free.
 * 
 * @param mbi the multiboot info structure
*/
void mm_init(multiboot_info_t* mbi);


/**
 * Allocates a contiguous number pages.
 * 
 * @param num_pages the number of contiguous pages to allocate
 * 
 * @return the address of the first page
*/
void* alloc_pages(size_t num_pages);

/**
 * Frees a contiguous number of pages.
 * 
 * @param page_addr the address of the first page
 * @param num_pages the number of contiguous
*/
void free_pages(void* page_addr, size_t num_pages);
