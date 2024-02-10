#include <kernel/memory.h>
#include <kernel/utils.h>
#include <stdio.h>
#include <stdint.h>


extern char _kernel_end_physical;

#define KERNEL_END 		((unsigned long) &_kernel_end_physical)
#define KERNEL_END_PAGE	(KERNEL_END / PAGE_SIZE)

#define PAGE_SIZE 4096

/* Initialize all pages as "used" */
static uint32_t page_bitmap[1024] = {[0 ... 1023] = 0xFFFFFFFF};	// TODO: create dinamically, exclude grub and kernel pages


void memory_detect(multiboot_info_t* mbi)
{
    for(unsigned i = 0; i < mbi->mmap_length; ) {
        multiboot_memory_map_t* mmm = (multiboot_memory_map_t*) (mbi->mmap_addr + i + KERNEL_OFFSET);
 
        if(mmm->type == MULTIBOOT_MEMORY_AVAILABLE && (mmm->addr_low + mmm->len_low) >= KERNEL_END) {
			int page_start = DIV_ROUND_UP(MAX(mmm->addr_low, KERNEL_END), PAGE_SIZE);
			int page_end = (mmm->addr_low + mmm->len_low) / PAGE_SIZE;	// non-including

			// Mark pages as free (1) in the bitmap
			int page_start_array_offset = page_start / 32;
			int page_end_array_offset = page_end / 32;
			int page_start_bit_offset = page_start % 32;
			int page_end_bit_offset = page_end % 32;

			if(page_start_array_offset == page_end_array_offset) {
				page_bitmap[page_start_array_offset] = ~((0xFFFFFFFF << page_start_bit_offset) & (0xFFFFFFFF >> (32 - page_end_bit_offset)));
			}
			else {
				page_bitmap[page_start_array_offset] = ~(0xFFFFFFFF << page_start_bit_offset);
				page_bitmap[page_end_array_offset] = 0xFFFFFFFF << page_end_bit_offset;

				for(int j = page_start_array_offset + 1; j < page_end_array_offset; j++)
					page_bitmap[j] = 0;
			}
        }

		i += mmm->size + sizeof(mmm->size);
    }
}
