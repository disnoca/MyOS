/**
 * Code for setting up Paging.
 * 
 * Refer to:
 * Intel Software Developer Manual, Volume 3-A: Chapter 4.3: 32-Bit Paging
 * https://wiki.osdev.org/Paging
 * https://wiki.osdev.org/Setting_Up_Paging
 * 
 * @author Samuel Pires
*/

#include <kernel/paging.h>

#include <stdint.h>
#include <stdio.h>

#define PDE_COUNT 1024u
#define PTE_COUNT 1024u

static uint32_t page_directory[PDE_COUNT] __attribute__((aligned(4096)));
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

static uint32_t page_table[PDE_COUNT][PTE_COUNT] __attribute__((aligned(4096)));
/**
 * Page Table Entry structure:
 * 	bits 31:12 - Address of page
 * 	bits 11:9  - Ignored
 * 	bit 8      - Global
 * 	bit 7      - Page Attribute Table
 * 	bit 6      - Ignored
 * 	bit 5      - Accessed (set by CPU)
 * 	bit 4      - Dirty (set by CPU)
 * 	bit 3      - Write through
 * 	bit 2      - User/Supervisor
 * 	bit 1      - Read/Write
 * 	bit 0      - Present
*/


/**
 * Encodes identity paging into the page directory and their corresponding page tables.
*/
static void encode_identity_paging(void)
{
	/* Map the full 4GB of memory linearly to the page table entries and add them
	   to the corresponding page directory entries. */
	for(unsigned int i = 0; i < PDE_COUNT; i++) {
		page_directory[i] = ((unsigned int) (page_table + i)) | 0x3;

		for(unsigned int j = 0; j < PTE_COUNT; j++)
			page_table[i][j] = ((i * PTE_COUNT + j) * 4096) | 0x3;
	}
}

void paging_enable(void)
{
	encode_identity_paging();

	/* Load the page directory into the CR3 register. */
	asm volatile("mov cr3, %0" : : "r" (page_directory));

	/* Enable support for 4MB pages */
	asm volatile("mov eax, cr4\n\t"
				"or eax, 0x00000010\n\t"
				"mov cr4, eax" : : : "eax");

	/* Enable paging by setting the paging bit in the CR0 register. */
	asm volatile("mov eax, cr0\n\t"
				"or eax, 0x80000000\n\t"
				"mov cr0, eax" : : : "eax");
}
