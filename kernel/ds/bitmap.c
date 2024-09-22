#include <stdio.h>
#include <limits.h>

#include <kernel/ds/bitmap.h>
#include <kernel/utils.h>


#define BITMAP_ENTRIES_PER_ARR_ELEM		(sizeof(bitmap_array_elem_t) * 8)
#define BITMAP_ARRAY_ELEM_MAX				((bitmap_array_elem_t)ULONG_MAX)
#define BITMAP_ENTRY_TO_ARR_INDEX(entry)	((entry) / BITMAP_ENTRIES_PER_ARR_ELEM)
#define BITMAP_ENTRY_TO_BIT_OFFSET(entry)	((entry) % BITMAP_ENTRIES_PER_ARR_ELEM)


static void set_entries_used(bitmap_array_elem_t* bitmap, size_t bitmap_entry, size_t nentries);


unsigned long bitmap_alloc(bitmap_array_elem_t* bitmap, size_t size, size_t nentries, size_t* maxnfound)
{
	if (maxnfound)
		*maxnfound = 0;

	unsigned long total_entries = size * 8;

	/* Return 0 if there aren't enough entries to satisfy the request */
	if (nentries > total_entries)
		return 0;

	unsigned long curr_entry = 0;
	size_t nfound = 0;

	for (size_t i = 0; curr_entry + nfound < total_entries; i++)
	{
		bitmap_array_elem_t arr_entry = bitmap[i];
	
		/* Skip array entries with no free bitmap entries */
		if (!((bitmap_array_elem_t)~arr_entry)) {
			nfound = 0;
			curr_entry = (i + 1) * BITMAP_ENTRIES_PER_ARR_ELEM;
			continue;
		}

		bitmap_array_elem_t search_mask = 1;

		for (size_t j = 0; j < BITMAP_ENTRIES_PER_ARR_ELEM; j++)
		{
			if (~arr_entry & search_mask) {
				nfound++;
				if (maxnfound && nfound > *maxnfound)
					*maxnfound = nfound;
			} else {
				nfound = 0;
				curr_entry = i * BITMAP_ENTRIES_PER_ARR_ELEM + j + 1;
			}

			if (nfound == nentries) {
				set_entries_used(bitmap, curr_entry, nentries);
				return curr_entry;
			}

			search_mask <<= 1;
		}
	}

	return 0;
}

void bitmap_free(bitmap_array_elem_t* bitmap, size_t bitmap_entry, size_t nentries)
{
	size_t array_entry = BITMAP_ENTRY_TO_ARR_INDEX(bitmap_entry);
	size_t bit_offset = BITMAP_ENTRY_TO_BIT_OFFSET(bitmap_entry);

	while (nentries > 0)
	{
		size_t bits_to_zero = MIN(BITMAP_ENTRIES_PER_ARR_ELEM - bit_offset, nentries);

		bitmap_array_elem_t mask = bits_to_zero == BITMAP_ENTRIES_PER_ARR_ELEM ? BITMAP_ARRAY_ELEM_MAX :
			~(BITMAP_ARRAY_ELEM_MAX << bits_to_zero) << bit_offset;
		bitmap[array_entry] &= ~mask;

		nentries -= bits_to_zero;
		array_entry++;
		bit_offset = 0;
	}
}



static void set_entries_used(bitmap_array_elem_t* bitmap, size_t bitmap_entry, size_t nentries)
{
	size_t array_entry = BITMAP_ENTRY_TO_ARR_INDEX(bitmap_entry);
	size_t bit_offset = BITMAP_ENTRY_TO_BIT_OFFSET(bitmap_entry);

	while (nentries > 0)
	{
		size_t bits_to_set = MIN(BITMAP_ENTRIES_PER_ARR_ELEM - bit_offset, nentries);

		bitmap_array_elem_t mask = bits_to_set == BITMAP_ENTRIES_PER_ARR_ELEM ? BITMAP_ARRAY_ELEM_MAX :
			~(BITMAP_ARRAY_ELEM_MAX << bits_to_set) << bit_offset;
		bitmap[array_entry] |= mask;

		nentries -= bits_to_set;
		array_entry++;
		bit_offset = 0;
	}
}
