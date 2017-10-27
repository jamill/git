#include "cache.h"
#include "cache-entry-manager.h"

static struct mem_pool *mem_pool = 0;
#define CACHE_ENTRY_PATH_LENGTH 40

/*
 * Set parameters on memory set aside for cache entries
 */
void cache_entry_manager_set_count_hint(size_t num_entries)
{
	// trace_printf("set_cache_entry_size: num entries: %"PRIuMAX, (uintmax_t)num_entries);
	mem_pool_init(&mem_pool, 0, num_entries * (sizeof(struct cache_entry) + CACHE_ENTRY_PATH_LENGTH));
}

/*
 * Alloc a cache_entry
 */
void *cache_entry_alloc(size_t len)
{
	mem_pool_init(&mem_pool, 0, 0);
	return mem_pool_alloc(mem_pool, cache_entry_size(len));
}

void * cache_entry_calloc(size_t len)
{
	mem_pool_init(&mem_pool, 0, 0);
	return mem_pool_calloc(mem_pool, cache_entry_size(len));
}

/*
 * Free an allocated cache_entry
 */
void cache_entry_free(struct cache_entry *ce)
{
	// trace_printf("trace: cache_entry_free. mem_manager: %p, free: %p", mem_manager, ce);
	// If this memory pool has allocated this memory, then do not free it.
	if (mem_pool &&
	    !mem_pool_contains(mem_pool, ce)) {
		free(ce);
	}
}
