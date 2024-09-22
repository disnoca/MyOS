/**
 * Code for the page allocation bitmap.
 * 
 * @author Samuel Pires
*/

#include <kernel/utils.h>
#include <kernel/mm/mm.h>
#include <kernel/system.h>

/* Must be defined: PAGE_SIZE */
#ifdef __i386__
#include <kernel/arch/i386/paging.h>
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <limits.h>


typedef unsigned long bmap_array_elem_t;

static bmap_array_elem_t* bmap;

static size_t bmap_arr_length;

static uintptr_t mem_start, mem_end;


#define PAGES_PER_ARR_ELEM	(8 * sizeof(bmap_array_elem_t))
#define BMAP_ARRAY_ELEM_MAX	((bmap_array_elem_t)ULONG_MAX)

#define MEM_START_PAGE	(mem_start / PAGE_SIZE)

/* The bitmap does not include the static kernel or itself, therefore a page with index x corresponds to entry x - excluded pages. */
/* Here are some macros to help with conversion: */
#define PFN_TO_BMAP_ENTRY(pi) 			((pi) - MEM_START_PAGE)
#define PAGE_ADDR_TO_BMAP_ENTRY(pa) 	(((pa) / PAGE_SIZE) - MEM_START_PAGE)
#define BMAP_ENTRY_TO_PFN(be) 			((be) + MEM_START_PAGE)
#define BMAP_ENTRY_TO_PAGE_ADDR(be)		(((be) + MEM_START_PAGE) * PAGE_SIZE)

#define BMAP_ENTRY_TO_ARR_IDX(be)		((be) / PAGES_PER_ARR_ELEM)
#define BMAP_ENTRY_TO_BIT_OFFSET(be)	((be) % PAGES_PER_ARR_ELEM)


uintptr_t bmap_init(uintptr_t static_kernel_end, uintptr_t mem_end);
void bmap_exclude(uintptr_t start_addr, uintptr_t end_addr);

void* bmap_alloc(size_t num_pages);
void* bmap_alloc_lower(size_t num_pages, uintptr_t addr_lower);
void* bmap_alloc_upper(size_t num_pages, uintptr_t addr_upper);
void* bmap_alloc_range(size_t num_pages, uintptr_t addr_lower, uintptr_t addr_upper);

void bmap_free(void* page_addr, size_t num_pages);
void bmap_print(void);

static void set_pages_used(uintptr_t page_addr, size_t num_pages);
static void set_pages_free(uintptr_t page_addr, size_t num_pages);


/* Global Functions */

/**
 * Allocates an initializes the bitmap.
 * 
 * This function assumes that all memory after mem_start is free at the time of the call.
 * 
 * @param mem_start the start address of the memory to allocate the bitmap
 * @param mem_end the end address of the memory
 * 
 * @return the start address of available memory
*/
uintptr_t bmap_init(uintptr_t _mem_start, uintptr_t _mem_end)
{
	mem_start = ALIGN_UP(_mem_start, sizeof(bmap_array_elem_t));
	mem_end = ALIGN_DOWN(_mem_end, PAGE_SIZE);

	bmap = (bmap_array_elem_t*) P2V(mem_start);

	uintptr_t aligned_free_mem_start = ALIGN_UP(mem_start, PAGE_SIZE);

	size_t free_mem_size = mem_end - aligned_free_mem_start;
	size_t num_free_pages = free_mem_size / PAGE_SIZE;
	size_t bmap_size = ALIGN_UP(num_free_pages, PAGES_PER_ARR_ELEM) / 8;

	mem_start += bmap_size;

	/* If bitmap occupies previously considered free pages */
	if (mem_start > aligned_free_mem_start)
	{
		size_t additional_bmap_mem = mem_start - aligned_free_mem_start;

		aligned_free_mem_start += ALIGN_UP(additional_bmap_mem, PAGE_SIZE);

		/* Exclude additional memory the bitmap will occupy */
		free_mem_size = mem_end - aligned_free_mem_start;
		num_free_pages = free_mem_size / PAGE_SIZE;
		bmap_size = ALIGN_UP(num_free_pages, PAGES_PER_ARR_ELEM) / 8;

		// TODO: reclaim page that might become available due to the bitmap's size reduction
	}

	mem_start = aligned_free_mem_start;
	bmap_arr_length = bmap_size / sizeof(bmap_array_elem_t);

	if (mem_start >= mem_end) {
		PANIC("Not enough memory to allocate bitmap");
	}

	/* Set all pages as free */
	for (unsigned int i = 0; i < bmap_arr_length - 1; i++)
		bmap[i] = 0;

	/* Set leftover pages as used */
	bmap[bmap_arr_length - 1] = ~(BMAP_ARRAY_ELEM_MAX >> (num_free_pages % PAGES_PER_ARR_ELEM));

	return mem_start;
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
	start_addr = ALIGN_DOWN(start_addr, PAGE_SIZE);
	end_addr = ALIGN_UP(end_addr, PAGE_SIZE);

	set_pages_used(start_addr, (end_addr - start_addr) / PAGE_SIZE);
}


/**
 * Allocates contiguous pages.
 * 
 * @param num_pages the number of pages to allocate
 * 
 * @return the memory address of the first page or NULL if there aren't enough
 * free contiguous pages anywhere in memory
 */
void* bmap_alloc(size_t num_pages)
{
	return bmap_alloc_range(num_pages, mem_start, mem_end);
}

/**
 * Allocates contiguous pages above a specified address.
 * 
 * @param num_pages the number of pages to allocate
 * @param addr_lower the address at which to allocate pages above of
 * 
 * @return the memory address of the first page or NULL if there aren't enough
 * free contiguous pages above the specified address.
 */
void* bmap_alloc_lower(size_t num_pages, uintptr_t addr_lower)
{
	return bmap_alloc_range(num_pages, addr_lower, mem_end);
}

/**
 * Allocates contiguous pages below a specified address.
 * 
 * @param num_pages the number of pages to allocate
 * @param addr_upper the address at which to allocate pages below of
 * 
 * @return the memory address of the first page or NULL if there aren't enough
 * free contiguous pages below the specified address.
 */
void* bmap_alloc_upper(size_t num_pages, uintptr_t addr_upper)
{
	return bmap_alloc_range(num_pages, mem_start, addr_upper);
}

/**
 * Allocates contiguous pages within a specified range.
 * 
 * @param num_pages the number of pages to allocate
 * @param addr_lower the lower bound address of the range, inclusive
 * @param max_addr the upper bound address of the range, exclusive
 * 
 * @return the memory address of the first page or NULL if there aren't enough
 * free contiguous pages within the specified range to satisfy the request.
*/
void* bmap_alloc_range(size_t num_pages, uintptr_t addr_lower, uintptr_t addr_upper)
{
	if (num_pages == 0)
		return NULL;

	/* Return NULL if the specified range doesn't contain memory */
	if (addr_lower >= mem_end || addr_upper <= mem_start)
		return NULL;

	addr_lower = MAX(addr_lower, mem_start);
	addr_upper = MIN(addr_upper, mem_end);

	/* Return NULL if there isn't enough memory in the specified range to satisfy the request */
	if ((addr_upper - addr_lower) / PAGE_SIZE < num_pages)
		return NULL;

	unsigned long start_entry = PAGE_ADDR_TO_BMAP_ENTRY(addr_lower);
	unsigned long end_entry = PAGE_ADDR_TO_BMAP_ENTRY(addr_upper);

	unsigned long curr_entry = start_entry;
	size_t contiguous_pages_found = 0;

	/* Only search within the specified range */
	for (size_t i = BMAP_ENTRY_TO_ARR_IDX(start_entry); curr_entry + contiguous_pages_found < end_entry; i++)
	{
		bmap_array_elem_t arr_elem = bmap[i];

		/* Skip array elements with no free bitmap entries */
		if (!((bmap_array_elem_t)~arr_elem)) {
			contiguous_pages_found = 0;
			curr_entry = (i + 1) * PAGES_PER_ARR_ELEM;
			continue;
		}

		bmap_array_elem_t search_mask = 1;

		for (size_t j = 0; j < PAGES_PER_ARR_ELEM; j++)
		{
			/* If it's the first iteration, adjust the starting bit */
			if (i == BMAP_ENTRY_TO_ARR_IDX(start_entry) && j == 0) {
				j = BMAP_ENTRY_TO_BIT_OFFSET(start_entry);
				search_mask <<= j;
			}

			if (~arr_elem & search_mask) {
				contiguous_pages_found++;
			} else {
				contiguous_pages_found = 0;
				curr_entry = i * PAGES_PER_ARR_ELEM + j + 1;
			}

			if (contiguous_pages_found == num_pages) {
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
	if (page_addr == NULL)
		return;

	set_pages_free((uintptr_t) page_addr, num_pages);
}


/**
 * Prints the bitmap's contents.
*/
void bmap_print(void)
{
	printf("addr start: %x\n", mem_start);
	printf("addr end: %x\n", mem_end);
	printf("entry start: %u\n", PAGE_ADDR_TO_BMAP_ENTRY(mem_start));
	printf("entry end: %u\n", PAGE_ADDR_TO_BMAP_ENTRY(mem_end));
	printf("array size: %u\n", bmap_arr_length);

	for (size_t i = 0; i < bmap_arr_length; i++)
		printf("0x%X ", bmap[i]);
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
	size_t arr_idx = BMAP_ENTRY_TO_ARR_IDX(bmap_entry);
	size_t bit_offset = BMAP_ENTRY_TO_BIT_OFFSET(bmap_entry);

	while (num_pages > 0)
	{
		size_t bits_to_set = MIN(PAGES_PER_ARR_ELEM - bit_offset, num_pages);

		bmap_array_elem_t mask = bits_to_set == PAGES_PER_ARR_ELEM ? BMAP_ARRAY_ELEM_MAX :
			~(BMAP_ARRAY_ELEM_MAX << bits_to_set) << bit_offset;
		bmap[arr_idx] |= mask;

		num_pages -= bits_to_set;
		arr_idx++;
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
	size_t arr_idx = BMAP_ENTRY_TO_ARR_IDX(bmap_entry);
	size_t bit_offset = BMAP_ENTRY_TO_BIT_OFFSET(bmap_entry);

	while (num_pages > 0)
	{
		size_t bits_to_zero = MIN(PAGES_PER_ARR_ELEM - bit_offset, num_pages);

		bmap_array_elem_t mask = bits_to_zero == PAGES_PER_ARR_ELEM ? BMAP_ARRAY_ELEM_MAX :
			~(BMAP_ARRAY_ELEM_MAX << bits_to_zero) << bit_offset;
		bmap[arr_idx] &= ~mask;

		num_pages -= bits_to_zero;
		arr_idx++;
		bit_offset = 0;
	}
}
