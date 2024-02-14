#include <kernel/memory.h>
#include <kernel/utils.h>
#include <kernel/system.h>
#include <kernel/paging.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


extern char _kernel_end_physical;

extern uint32_t boot_page_directory[];


/* Kernel space includes allocated memory bitmap */
static unsigned long user_start_addr;
static unsigned long user_start_block;

static unsigned long memory_addr_max;


#define BLOCK_SIZE 4096

typedef unsigned long mem_bmap_array_elem_t;

#define MEM_BMAP_BLOCKS_PER_ARRAY_ENTRY	(8 * sizeof(mem_bmap_array_elem_t))
#define MEM_BMAP_ARRAY_ELEM_MAX			0xFFFFFFFF

static mem_bmap_array_elem_t* mem_bmap;

size_t mem_bmap_array_length;

/* The memory bitmap only includes user memory, meaning index x corresponds to block x + user_start_block
   Here are some macros to help with conversion: */
#define BLOCK_INDEX_TO_BMAP_ENTRY(pi) 	(pi - user_start_block)
#define BLOCK_ADDR_TO_BMAP_ENTRY(pa) 	((pa / BLOCK_SIZE) - user_start_block)
#define BMAP_ENTRY_TO_BLOCK_INDEX(be) 	(be + user_start_block)
#define BMAP_ENTRY_TO_BLOCK_ADDR(be)	((be + user_start_block) * BLOCK_SIZE)


/**
 * Calculates the amount of available memory and sets the corresponding global variable accordingly.
 * 
 * Initializes the global variables memory_size and memory_addr_max.
 * 
 * @param mbi the multiboot info struct
*/
static void memory_detect_size(multiboot_info_t* mbi);

/**
 * Allocates the memory bitmap, calculates the end of static kernel memory / start of user memory
 * and sets the corresponding global variables accordingly.
 * 
 * Initializes the global variables: mem_bmap_array_length, user_start_addr and user_start_block.
 * 
 * Assumes that memory_detect_size has been called beforehand and that there is enough sequential memory
 * after the kernel to allocate the memory bitmap.
*/
static void memory_bitmap_allocate();

/**
 * Initializes the bitmap with the information given in the multiboot memory map.
 * 
 * Assumes that memory_bitmap_allocate has been called beforehand.
 * 
 * @param mbi the multiboot info struct
*/
static void memory_bitmap_init(multiboot_info_t* mbi);



/* Header Implementation */

void memory_detect(multiboot_info_t* mbi)
{
	memory_detect_size(mbi);
	memory_bitmap_allocate();
	memory_bitmap_init(mbi);
}


size_t mblocks_alloc(unsigned long* addrs, size_t count)
{
	if(count == 0)
		return 0;

	size_t allocs = 0;

	for(size_t i = 0; i < mem_bmap_array_length; i++)
	{
		if(~mem_bmap[i])	// Corresponding array member has at least one free block
		{
			for(size_t j = 0; j < MEM_BMAP_BLOCKS_PER_ARRAY_ENTRY; j++)
			{
				mem_bmap_array_elem_t block_mask = 1 << j;
				if(!(block_mask & mem_bmap[i]))
				{
					addrs[allocs++] = BMAP_ENTRY_TO_BLOCK_ADDR(i * MEM_BMAP_BLOCKS_PER_ARRAY_ENTRY + j);
					mem_bmap[i] |= block_mask;

					if(allocs == count)
						return count;
				}
			}
		}
	}

	return allocs;
}

void mblocks_free(const unsigned long* addrs, size_t count)
{
	for(size_t i = 0; i < count; i++) {
		unsigned long bmap_entry = BLOCK_ADDR_TO_BMAP_ENTRY(addrs[i]);
		mem_bmap_array_elem_t block_mask = 1 << (bmap_entry % MEM_BMAP_BLOCKS_PER_ARRAY_ENTRY);
		mem_bmap[bmap_entry / MEM_BMAP_BLOCKS_PER_ARRAY_ENTRY] &= ~block_mask;
	}
}



/* Helper Functions */

static void memory_detect_size(multiboot_info_t* mbi)
{
	memory_addr_max = 0;

	for(unsigned int i = 0; i < mbi->mmap_length; ) {
        multiboot_memory_map_t* mmm = (multiboot_memory_map_t*) (mbi->mmap_addr + i + KERNEL_OFFSET);

		if(mmm->type == MULTIBOOT_MEMORY_AVAILABLE) {
			memory_addr_max = MAX(memory_addr_max, mmm->addr_low + mmm->len_low);
		}

		i += mmm->size + sizeof(mmm->size);
	}

	if(memory_addr_max == 0) {
		PANIC("No memory found in multiboot memory map");
	}
}


static void memory_bitmap_allocate()
{
	unsigned long curr_kernel_endp = (unsigned long) &_kernel_end_physical;
	curr_kernel_endp = ROUND_UP(curr_kernel_endp, sizeof(mem_bmap_array_elem_t));

	unsigned long bitmap_entries_count = DIV_ROUND_UP(memory_addr_max - curr_kernel_endp, BLOCK_SIZE);
	
	/* The actual value might change during the method but this global variable is updated in the end */
	mem_bmap_array_length = DIV_ROUND_UP(bitmap_entries_count, MEM_BMAP_BLOCKS_PER_ARRAY_ENTRY);
	
	unsigned int first_boot_page_table_index = ADDR_TO_PAGE_TABLE_INDEX(KERNEL_OFFSET);
	unsigned int last_boot_page_table_index = first_boot_page_table_index;
	while(boot_page_directory[last_boot_page_table_index + 1] != 0)
		last_boot_page_table_index++;

	unsigned long remaining_allocated_memory_size = ROUND_UP(curr_kernel_endp, BLOCK_SIZE) - curr_kernel_endp;
	unsigned long required_memory_size = mem_bmap_array_length * sizeof(mem_bmap_array_elem_t) > remaining_allocated_memory_size ?
		mem_bmap_array_length * sizeof(mem_bmap_array_elem_t) - remaining_allocated_memory_size : 0;

	/* If the allocation of the memory bitmap requires at least one new page table, align curr_kernel_endp
	   for later creation of page tables. */
	if(ADDR_TO_PAGE_TABLE_INDEX(P2V(curr_kernel_endp + required_memory_size)) > last_boot_page_table_index) {
		curr_kernel_endp += remaining_allocated_memory_size;
		required_memory_size += remaining_allocated_memory_size;
	}

	if(required_memory_size > 0)
	{
		/* For now, assume there are at least 4kb remaining in the kernel's last boot page table.
	   Don't forget to change it later to use the emergency page table in case there isn't. */

		unsigned int curr_page_directory_entry_index = last_boot_page_table_index;
		uint32_t* curr_page_table = (uint32_t*) (PAGE_DIRECTORY_ENTRY_ADDR(boot_page_directory[curr_page_directory_entry_index]) + KERNEL_OFFSET);
		unsigned int curr_page_index = DIV_ROUND_UP(curr_kernel_endp % PAGE_TABLE_ADDRESSABLE_RANGE, PAGE_SIZE);
		unsigned long new_allocated_memory_size = 0;

		/* Map new page table entries until all necessary memory is paged */
		while(new_allocated_memory_size < required_memory_size) {
			curr_page_table[curr_page_index++] = PAGE_TABLE_ENTRY_CREATE(curr_kernel_endp, PAGE_PRESENT | PAGE_WRITE);

			/* Create new page table if the current one gets filled */
			if(curr_page_index == PAGE_TABLE_NUM_ENTRIES) {
				curr_page_index = 0;

				boot_page_directory[++curr_page_directory_entry_index] = PAGE_DIRECTORY_ENTRY_CREATE(curr_kernel_endp, PAGE_PRESENT | PAGE_WRITE);

				curr_kernel_endp += PAGE_TABLE_SIZE;
				required_memory_size += PAGE_TABLE_SIZE;
				if(--bitmap_entries_count % MEM_BMAP_BLOCKS_PER_ARRAY_ENTRY == 0) {
					mem_bmap_array_length--;
					required_memory_size -= sizeof(mem_bmap_array_elem_t);
				}
			}

			new_allocated_memory_size += PAGE_SIZE;
		}
	}

	mem_bmap = P2V(curr_kernel_endp);
	user_start_addr = curr_kernel_endp + mem_bmap_array_length * sizeof(mem_bmap_array_elem_t);
	user_start_block = DIV_ROUND_UP(user_start_addr, BLOCK_SIZE);
}

static void memory_bitmap_init(multiboot_info_t* mbi)
{
	for(unsigned int i = 0; i < mbi->mmap_length; ) {
		multiboot_memory_map_t* mmm = (multiboot_memory_map_t*) (mbi->mmap_addr + i + KERNEL_OFFSET);
 
		if(mmm->type == MULTIBOOT_MEMORY_AVAILABLE && (mmm->addr_low + mmm->len_low) >= user_start_addr) {
			size_t block_start = DIV_ROUND_UP(MAX(mmm->addr_low, user_start_addr), BLOCK_SIZE);
			size_t block_end = (mmm->addr_low + mmm->len_low) / BLOCK_SIZE;	// non-including

			// Mark blocks as free (0) in the bitmap
			size_t block_start_array_offset = BLOCK_INDEX_TO_BMAP_ENTRY(block_start) / MEM_BMAP_BLOCKS_PER_ARRAY_ENTRY;
			size_t block_end_array_offset = BLOCK_INDEX_TO_BMAP_ENTRY(block_end) / MEM_BMAP_BLOCKS_PER_ARRAY_ENTRY;
			size_t block_start_bit_offset = BLOCK_INDEX_TO_BMAP_ENTRY(block_start) % MEM_BMAP_BLOCKS_PER_ARRAY_ENTRY;
			size_t block_end_bit_offset = BLOCK_INDEX_TO_BMAP_ENTRY(block_end) % MEM_BMAP_BLOCKS_PER_ARRAY_ENTRY;

			if(block_start_array_offset == block_end_array_offset) {
				mem_bmap[block_start_array_offset] = ~((MEM_BMAP_ARRAY_ELEM_MAX << block_start_bit_offset) & (MEM_BMAP_ARRAY_ELEM_MAX >> (MEM_BMAP_BLOCKS_PER_ARRAY_ENTRY - block_end_bit_offset)));
			}
			else {
				mem_bmap[block_start_array_offset] = ~(MEM_BMAP_ARRAY_ELEM_MAX << block_start_bit_offset);
				mem_bmap[block_end_array_offset] = MEM_BMAP_ARRAY_ELEM_MAX << block_end_bit_offset;

				for(size_t j = block_start_array_offset + 1; j < block_end_array_offset; j++)
					mem_bmap[j] = 0;
			}
		}

		i += mmm->size + sizeof(mmm->size);
	}
}
