#pragma once

#include <stdint.h>
#include <stddef.h>


typedef uint32_t bitmap_array_elem_t;
typedef bitmap_array_elem_t bitmap_t;


/**
 * Allocates contiguous entries in a bitmap.
 * 
 * The bitmap must be aligned to sizeof(bitmap_array_elem_t) bytes.
 * 
 * If 0 is returned and it's a valid entry, nfound must be checked to
 * know if the allocation succeeded.
 * 
 * @param bitmap the bitmap to allocate from
 * @param size the size of the bitmap in bytes
 * @param nentries the number of entries to allocate
 * @param nfound output pointer to the maximum number of contiguous entries found (can be NULL)
 * 
 * @return the index of the first entry or 0 if not enough contiguous entries were found
*/
unsigned long bitmap_alloc(bitmap_t* bitmap, size_t size, size_t nentries, size_t* nfound);

/**
 * Frees contiguous entries in a bitmap.
 * 
 * The bitmap must be aligned to sizeof(bitmap_array_elem_t) bytes.
 * 
 * @param bitmap the bitmap to free from
 * @param bitmap_entry the index of the first bitmap entry
 * @param nentries the number of entries to free
 */
void bitmap_free(bitmap_t* bitmap, size_t bitmap_entry, size_t nentries);
