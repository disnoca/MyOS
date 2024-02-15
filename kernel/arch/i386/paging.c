#include <kernel/arch/i386/paging.h>


void page_directory_set_page(uint32_t* pd, unsigned long paddr, unsigned long vaddr, uint16_t flags)
{
	/* Make sure to check if there's at least 4KB of free paged space to
	   allocate the new page table, use emergency page table otherwise */
	/* Dont forget to align page tables to 4KB when allocating them */
}


void page_directory_remove_page(uint32_t* pd, unsigned long addr)
{
	/* In case the corresponding page table only has one remaining valid entry,
	   check if that entry contains itself and free it if so. */
}
