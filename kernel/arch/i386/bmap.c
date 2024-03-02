#include <kernel/arch/i386/mm.h>
#include <kernel/arch/i386/paging.h>
#include <kernel/arch/i386/system.h>
#include <kernel/utils.h>

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>


typedef unsigned long bmap_array_elem_t;

static bmap_array_elem_t* bmap;

static size_t bmap_arr_length;

static uintptr_t free_mem_start;
static size_t free_mem_start_page;


#define PAGES_PER_ARRAY_ENTRY			(8 * sizeof(bmap_array_elem_t))
#define BMAP_ARRAY_ELEM_MAX				0xFFFFFFFFUL

/* The bitmap does not include the static kernel or itself, therefore a page with index x corresponds to entry x - excluded pages. */
/* Here are some macros to help with conversion: */
#define PAGE_INDEX_TO_BMAP_ENTRY(pi) 	(pi - free_mem_start_page)
#define PAGE_ADDR_TO_BMAP_ENTRY(pa) 	((pa / PAGE_SIZE) - free_mem_start_page)
#define BMAP_ENTRY_TO_PAGE_INDEX(be) 	(be + free_mem_start_page)
#define BMAP_ENTRY_TO_PAGE_ADDR(be)		((be + free_mem_start_page) * PAGE_SIZE)


uintptr_t bmap_init(uintptr_t static_kernel_end, uintptr_t mem_end);
void bmap_exclude(uintptr_t start_addr, uintptr_t end_addr);
void* bmap_alloc(size_t num_pages);
void bmap_free(void* page_addr, size_t num_pages);
void bmap_print(void);

static void set_pages_used(uintptr_t page_addr, size_t num_pages);
static void set_pages_free(uintptr_t page_addr, size_t num_pages);


/* Global Functions */

/**
 * Allocates an initializes the bitmap.
 * 
 * This function assumes that all memory after static kernel is free at the time of the call.
 * 
 * @param static_kernel_end the end address of static kernel memory
 * @param mem_end the end address of the memory
 * 
 * @return the start address of available memory
*/
uintptr_t bmap_init(uintptr_t static_kernel_end, uintptr_t mem_end)
{
	free_mem_start = ROUND_UP(static_kernel_end, sizeof(bmap_array_elem_t));
	mem_end = ROUND_DOWN(mem_end, PAGE_SIZE);

	bmap = (bmap_array_elem_t*) P2V(free_mem_start);

	uintptr_t aligned_free_mem_start = ROUND_UP(free_mem_start, PAGE_SIZE);

	size_t free_mem_size = mem_end - aligned_free_mem_start;
	size_t num_free_pages = free_mem_size / PAGE_SIZE;
	size_t bmap_size = ROUND_UP(num_free_pages, PAGES_PER_ARRAY_ENTRY) / 8;

	free_mem_start += bmap_size;

	/* If bitmap occupies previously considered free pages */
	if(free_mem_start > aligned_free_mem_start)
	{
		size_t additional_bmap_mem = free_mem_start - aligned_free_mem_start;

		aligned_free_mem_start += ROUND_UP(additional_bmap_mem, PAGE_SIZE);

		/* Exclude additional memory the bitmap will occupy */
		free_mem_size = mem_end - aligned_free_mem_start;
		num_free_pages = free_mem_size / PAGE_SIZE;
		bmap_size = ROUND_UP(num_free_pages, PAGES_PER_ARRAY_ENTRY) / 8;

		// TODO: reclaim page that might become available due to the bitmap's size reduction
	}

	free_mem_start = aligned_free_mem_start;
	free_mem_start_page = free_mem_start / PAGE_SIZE;
	bmap_arr_length = bmap_size / sizeof(bmap_array_elem_t);

	if(free_mem_start >= mem_end) {
		PANIC("Not enough memory to allocate bitmap");
	}

	/* Set all pages as free */
	for(unsigned int i = 0; i < bmap_arr_length - 1; i++)
		bmap[i] = 0;

	/* Set leftover pages as used */
	bmap[bmap_arr_length - 1] = ~(BMAP_ARRAY_ELEM_MAX >> (num_free_pages % PAGES_PER_ARRAY_ENTRY));

	return free_mem_start;
}


/**
 * Excludes a section of memory from the bitmap.
 * 
 * This function should only be called before any allocations have taken place.
 * 
 * @param start_addr the address from where to exclude the memory, inclusive
 * @param end_addr the address until where to exclude the memory, exclusive
*/
void bmap_exclude(uintptr_t start_addr, uintptr_t end_addr)
{
	start_addr = ROUND_DOWN(start_addr, PAGE_SIZE);
	end_addr = ROUND_UP(end_addr, PAGE_SIZE);

	set_pages_used(start_addr, (end_addr - start_addr) / PAGE_SIZE);
}


/**
 * Allocates contiguous pages.
 * 
 * @param num_pages the number of contiguous pages to allocate
 * 
 * @return the memory address of the first page or NULL if there
 * aren't enough free contiguous pages to satisfy the request.
*/
void* bmap_alloc(size_t num_pages)
{
	if(num_pages == 0)
		return NULL;

	size_t curr_entry = 0;
	size_t contiguous_pages_found = 0;

	for(size_t i = 0; i < bmap_arr_length; i++) {
		if(!(~bmap[i]))	continue;	/* Ignore array entries with no free pages */

		bmap_array_elem_t arr_entry = bmap[i];
		size_t search_mask = 1;

		for(size_t j = 0; j < PAGES_PER_ARRAY_ENTRY; j++)
		{
			if(~arr_entry & search_mask)
				contiguous_pages_found++;
			else {
				contiguous_pages_found = 0;
				curr_entry = i * PAGES_PER_ARRAY_ENTRY + j + 1;
			}

			if(contiguous_pages_found == num_pages) {
				uintptr_t page_addr = BMAP_ENTRY_TO_PAGE_ADDR(curr_entry);
				set_pages_used(page_addr, num_pages);
				return (void*) page_addr;
			}

			search_mask <<= 1;
		}
	}

	return NULL;
}


/**
 * Frees contiguous pages.
 * 
 * @param page_addr the address of the first page
 * @param num_pages the number of contiguous pages to free
*/
void bmap_free(void* page_addr, size_t num_pages)
{
	if(page_addr == NULL)
		return;

	set_pages_free((uintptr_t) page_addr, num_pages);
}


/**
 * Prints the bitmap's contents.
*/
void bmap_print(void)
{
	for(size_t i = 0; i < 1; i++)
		printf("0x%x ", bmap[i]);
	printf("\n");
}


/* Helper Functions */

/**
 * Sets contiguous pages as used.
 * 
 * @param page_addr the memory address of the first page
 * @param num_pages the number of contiguous pages to set as used
*/
static void set_pages_used(uintptr_t page_addr, size_t num_pages)
{
	size_t bmap_entry = PAGE_ADDR_TO_BMAP_ENTRY(page_addr);
	size_t array_entry = bmap_entry / PAGES_PER_ARRAY_ENTRY;
	size_t bit_offset = bmap_entry % PAGES_PER_ARRAY_ENTRY;

	while(num_pages > 0)
	{
		size_t bits_to_set = MIN(PAGES_PER_ARRAY_ENTRY - bit_offset, num_pages);

		bmap_array_elem_t mask = bits_to_set == PAGES_PER_ARRAY_ENTRY ? BMAP_ARRAY_ELEM_MAX :
			~(BMAP_ARRAY_ELEM_MAX << bits_to_set) << bit_offset;
		bmap[array_entry] |= mask;

		num_pages -= bits_to_set;
		array_entry++;
		bit_offset = 0;
	}
}

/**
 * Sets contiguous pages as free.
 * 
 * @param page_addr the memory address of the first page
 * @param num_pages the number of contiguous pages to set as free
*/
static void set_pages_free(uintptr_t page_addr, size_t num_pages)
{
	size_t bmap_entry = PAGE_ADDR_TO_BMAP_ENTRY(page_addr);
	size_t array_entry = bmap_entry / PAGES_PER_ARRAY_ENTRY;
	size_t bit_offset = bmap_entry % PAGES_PER_ARRAY_ENTRY;

	while(num_pages > 0)
	{
		size_t bits_to_zero = MIN(PAGES_PER_ARRAY_ENTRY - bit_offset, num_pages);

		bmap_array_elem_t mask = bits_to_zero == PAGES_PER_ARRAY_ENTRY ? BMAP_ARRAY_ELEM_MAX :
			~(BMAP_ARRAY_ELEM_MAX << bits_to_zero) << bit_offset;
		bmap[array_entry] &= ~mask;

		num_pages -= bits_to_zero;
		array_entry++;
		bit_offset = 0;
	}
}
