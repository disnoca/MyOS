#pragma once

#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE				4096

#define PD_ALIGNMENT			4096
#define PT_ALIGNMENT			4096

#define PDE_SIZE				4
#define PD_NUM_ENTRIES			1024
#define PD_SIZE					(PDE_SIZE * PD_NUM_ENTRIES)

#define PTE_SIZE				4
#define PT_NUM_ENTRIES 			1024
#define PT_SIZE 				(PTE_SIZE * PT_NUM_ENTRIES)
#define PT_ADDRESSABLE_RANGE	(PT_NUM_ENTRIES * PAGE_SIZE)

#define PDE_INDEX_TO_ADDR(i)	((i) * PT_ADDRESSABLE_RANGE)
#define ADDR_TO_PDE_INDEX(a)	((a) / PT_ADDRESSABLE_RANGE)

#define ADDR_TO_PTE_INDEX(a)	(((a) % PT_ADDRESSABLE_RANGE) / PAGE_SIZE)

#define PDE_ADDR_FIELD(pde) 	((pde) & 0xFFFFF000)
#define PTE_ADDR_FIELD(pte)		((unsigned long) (pte) & 0xFFFFF000)

#define PDE(a,f)				((a) | (f))
#define PTE(a,f)				((a) | (f))

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
 * Invalidates a page from the TLB.
 * 
 * @param addr the virtual address of the page to be invalidated
*/
static inline void tlb_invalidate_page(unsigned long addr) {
	asm volatile("invlpg [%0]" :: "r" (addr));
}
