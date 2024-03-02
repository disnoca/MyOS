#pragma once

#include <stdint.h>
#include <stddef.h>


/**
 * Initializes the slab allocator.
*/
void kmem_cache_init(void);

/**
 * Creates and returns a cache.
 * 
 * @param name the name of the cache
 * @param obj_size the size of the cache's objects
 * @param constructor the constructor function for the cache's objects
 * @param destructor the destructor function for the cache's objects
 * 
 * @return the created cache
*/
struct kmem_cache_s* kmem_cache_create(const char* name, size_t obj_size, void (*constructor)(void*, size_t), void (*destructor)(void*, size_t));

/**
 * Destroys a cache.
 * 
 * @param cache the cache to be destroyed
*/
void kmem_cache_destroy(struct kmem_cache_s* cache);

/**
 * Allocates a buffer from a cache.
 * 
 * @param cache the cache to allocate the buffer from
 * 
 * @return the allocated buffer's address or NULL if there are no available
 * buffers and not enough memory could be allocated to create a new slab
*/
void* kmem_cache_alloc(struct kmem_cache_s* cache);

/**
 * Frees a buffer of a cache.
 * 
 * @param cache the cache to free the buffer from
*/
void kmem_cache_free(struct kmem_cache_s* cache, void* bufp);

/**
 * Allocates a buffer from a cache.
 * 
 * @param cache the cache to allocate the buffer from
 * 
 * @return the allocated buffer's address or NULL if there are no available
 * buffers and not enough memory could be allocated to create a new slab
*/
void* kmem_alloc(size_t size);

/**
 * Frees a buffer of a cache.
 * 
 * @param cache the cache to free the buffer from
*/
void kmem_free(void* ptr);
