#include <kernel/utils.h>
#include <kernel/arch/i386/mm.h>
#include <kernel/arch/i386/system.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


extern char _kernel_end_physical;
extern uint32_t kernel_page_directory[];

extern uintptr_t bmap_init(uintptr_t static_kernel_end, uintptr_t mem_end);
extern void bmap_exclude(uintptr_t start_addr, uintptr_t end_addr);
extern void* bmap_alloc(size_t num_pages);
extern void bmap_free(void* page_addr, size_t num_pages);
extern void bmap_print(void);


static uintptr_t detect_mem_end(multiboot_info_t* mbi);
static bool mmm_exceeds_max_mem(multiboot_memory_map_t* mmm);


/* Global Functions */

void mm_init(multiboot_info_t* mbi)
{
	/* Check if there's a valid memory map */
    if(!(mbi->flags & MULTIBOOT_INFO_MEM_MAP)) {
        PANIC("Invalid memory map given by GRUB bootloader");
	}

	/* Initialize bitmap */	
	uintptr_t curr_addr = bmap_init((uintptr_t) &_kernel_end_physical, detect_mem_end(mbi));

	/* Detect and exclude memory holes */
	for(unsigned int i = 0; i < mbi->mmap_length; i += sizeof(multiboot_memory_map_t)) {
        multiboot_memory_map_t* mmm = (multiboot_memory_map_t*) P2V(mbi->mmap_addr + i);

		if(mmm->type == MULTIBOOT_MEMORY_AVAILABLE)
		{
			/* Ignore memory above 4GB */
			if(mmm_exceeds_max_mem(mmm))
				break;

			if(mmm->addr_low > curr_addr)
				bmap_exclude(curr_addr, mmm->addr_low);

			if(mmm->addr_low + mmm->len_low > curr_addr)
				curr_addr = mmm->addr_low + mmm->len_low;
		}
	}
}


void* alloc_pages(size_t num_pages)
{
	return bmap_alloc(num_pages);
}


void free_pages(void* page_addr, size_t num_pages)
{
	bmap_free(page_addr, num_pages);
}



/* Helper Functions */

/**
 * Detects and returns the end of the memory.
 * 
 * @param mbi the multiboot info struct
 * 
 * @return the end of memory address
*/
static uintptr_t detect_mem_end(multiboot_info_t* mbi)
{
	multiboot_memory_map_t* last_available_mmm = NULL;

	for(unsigned int i = 0; i < mbi->mmap_length; i += sizeof(multiboot_memory_map_t)) {
        multiboot_memory_map_t* mmm = (multiboot_memory_map_t*) P2V(mbi->mmap_addr + i);

		if(mmm->type == MULTIBOOT_MEMORY_AVAILABLE)
			last_available_mmm = mmm;
	}

	if(last_available_mmm == NULL) {
		PANIC("No usable memory found in multiboot memory map");
	}

	if(mmm_exceeds_max_mem(last_available_mmm))
		return 0xFFFFFFFF;

	return last_available_mmm->addr_low + last_available_mmm->len_low;
}


/**
 * Returns true if the given multiboot memory map entry exceeds max memory.
 * 
 * @param mmm the multiboot memory map entry
 * 
 * @return true if the given multiboot memory map entry exceeds max memory, false otherwise
*/
static bool mmm_exceeds_max_mem(multiboot_memory_map_t* mmm)
{
	return mmm->addr_high > 0 || mmm->len_high > 0 || UNSIGNED_SUM_OVERFLOWS(mmm->addr_low, mmm->len_low);
}
