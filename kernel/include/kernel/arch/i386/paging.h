#define pragma once

#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE							4096

#define PAGE_DIRECTORY_ALIGNMENT			4096
#define PAGE_TABLE_ALIGNMENT				4096

#define PAGE_DIRECTORY_ENTRY_SIZE			4
#define PAGE_DIRECTORY_NUM_ENTRIES			1024
#define PAGE_DIRECTORY_SIZE					(PAGE_DIRECTORY_ENTRY_SIZE * PAGE_DIRECTORY_NUM_ENTRIES)

#define PAGE_TABLE_ENTRY_SIZE				4
#define PAGE_TABLE_NUM_ENTRIES 				1024
#define PAGE_TABLE_SIZE 					(PAGE_TABLE_ENTRY_SIZE * PAGE_TABLE_NUM_ENTRIES)
#define PAGE_TABLE_ADDRESSABLE_RANGE		(PAGE_TABLE_NUM_ENTRIES * PAGE_SIZE)

#define PAGE_TABLE_INDEX_TO_ADDR(i)			((i) * PAGE_TABLE_ADDRESSABLE_RANGE)
#define ADDR_TO_PAGE_TABLE_INDEX(a)			((a) / PAGE_TABLE_ADDRESSABLE_RANGE)

#define ADDR_TO_PAGE_TABLE_ENTRY_OFFSET(a)	(((a) % PAGE_TABLE_ADDRESSABLE_RANGE) / PAGE_SIZE)

#define PAGE_DIRECTORY_ENTRY_ADDR(pde) 		((pde) & 0xFFFFF000)

#define PAGE_TABLE_ENTRY_ADDR(pte)			((unsigned long) (pte) & 0xFFFFF000)

#define PAGE_DIRECTORY_ENTRY_CREATE(a,f)	((a) | (f))
#define PAGE_TABLE_ENTRY_CREATE(a,f)		((a) | (f))

/**
 * Page Directory Entry structure:
 * 	bits 31:12 - Address of page table
 * 	bits 11:8  - Ignored
 * 	bit 7      - 0
 * 	bit 6      - Ignored
 * 	bit 5      - Accessed (set by CPU)
 * 	bit 4      - Cache disable
 * 	bit 3      - Write through
 * 	bit 2      - User/Supervisor
 * 	bit 1      - Read/Write
 * 	bit 0      - Present
*/

/**
 * Page Table Entry structure:
 * 	bits 31:12 - Address of page
 * 	bits 11:9  - Ignored
 * 	bit 8      - Global
 * 	bit 7      - Page Attribute Table
 * 	bit 6      - Dirty (set by CPU)
 * 	bit 5      - Accessed (set by CPU)
 *  bit 4      - Cache disable
 * 	bit 3      - Write through
 * 	bit 2      - User/Supervisor
 * 	bit 1      - Read/Write
 * 	bit 0      - Present
*/

/* PDE/PTE Flags */
#define PAGE_PRESENT 		(1 << 0)
#define PAGE_WRITE 			(1 << 1)
#define PAGE_USER 			(1 << 2)
#define PAGE_WRITETHROUGH 	(1 << 3)
#define PAGE_CACHEDISABLE 	(1 << 4)
#define PAGE_ACCESSED 		(1 << 5)
#define PAGE_DIRTY 			(1 << 6)
#define PAGE_PAT			(1 << 7)	/* Page Attribute Table */
#define PAGE_GLOBAL			(1 << 8)


/**
 * Adds a page to the specified page directory, overwriting any previous entries
 * and allocating new page tables if necessary.
 * 
 * @param pd a pointer to the start of the page directory
 * @param paddr the physical address to map the page to
 * @param vaddr the virtual address to map the page from
*/
void page_directory_set_page(uint32_t* pd, unsigned long paddr, unsigned long vaddr, uint16_t flags);

/**
 * Removes a page from the specified page directory, freeing the corresponding page table if it became empty.
 * 
 * @param pd a pointer to the start of the page directory
 * @param addr the virtual address of the page to be removed
*/
void page_directory_remove_page(uint32_t* pd, unsigned long addr);

/**
 * Invalidates a page from the TLB.
 * 
 * @param addr the virtual address of the page to be invalidated
*/
static inline void tlb_invalidate_page(unsigned long addr) {
	asm volatile("invlpg [%0]" :: "r" (addr));
}
