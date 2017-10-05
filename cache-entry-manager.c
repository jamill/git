#include "cache.h"
#include "cache-entry-manager.h"

static struct mem_pool_manager *mem_manager = 0;

void init_mem_manager(struct mem_pool_manager **mem_manager, size_t alloc_size)
{
	if (!(*mem_manager))
	{
		if (alloc_size < 1024 * 1024)
			alloc_size = 1024 * 1024;

		*mem_manager = xmalloc(sizeof(struct mem_pool_manager));
		(*mem_manager)->total_allocd = 0;
		(*mem_manager)->mem_pool = 0;
		(*mem_manager)->alloc_size = alloc_size;
	}
}

/*
 * Set parameters on memory set aside for cache entries
 */
void set_cache_entry_size(size_t num_entries)
{
	trace_printf("set_cache_entry_size: num entries: %"PRIuMAX, (uintmax_t)num_entries);
	init_mem_manager(&mem_manager, num_entries * (sizeof(struct cache_entry) + 40));
}

/*
 * Alloc a cache_entry - cache_entry + name are part of the
 * same struct
 */
void *cache_entry_alloc(size_t len)
{
	init_mem_manager(&mem_manager, 0);
	return mem_pool_alloc(mem_manager, cache_entry_size(len));
}

void cache_entry_free(struct cache_entry *ce)
{
	// trace_printf("trace: cache_entry_free. mem_manager: %p, free: %p", mem_manager, ce);
	if (mem_manager &&
	    !mem_pool_contains(mem_manager, ce)) {
		trace_printf("trace: free cache entry");
		free(ce);
	}
}
