#include "cache.h"
#include "cache_entry_manager.h"

static struct mem_pool_manager *mem_manager_combined = 0;
static struct mem_pool_manager *ce_mem_manager = 0;
static struct mem_pool_manager *path_mem_manager = 0;

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
	trace_printf("set_cache_entry_size: num entries: %lu", num_entries);
	init_mem_manager(&mem_manager_combined, num_entries * (sizeof(struct cache_entry) + 40));
}

/*
 * Alloc a cache_entry - cache_entry + name are part of the
 * same struct
 */
void *cache_entry_alloc(size_t len)
{
	init_mem_manager(&mem_manager_combined, 0);
	return memory_pool_alloc(mem_manager_combined, cache_entry_size(len));
}

void cache_entry_free(struct cache_entry *ce)
{
	// trace_printf("trace: cache_entry_free. mem_manager: %p, free: %p", mem_manager_combined, ce);
	if (mem_manager_combined &&
	    !memory_pool_contains(mem_manager_combined, ce)) {
		trace_printf("trace: free cache entry");
		free(ce);
	}
}

/*
 * Alloc a cache entry - cache entry struct and name portion come from
 * different memory pools.
 */
void *cache_entry_alloc_full2(size_t name_len)
{
	struct cache_entry *ce = memory_pool_alloc(ce_mem_manager, sizeof(struct cache_entry));
	// ce->name = cache_entry_allot_path(name_len);
	return ce;
}

/*
 * Alloc just the memory for a cache_entry name
 */
void *cache_entry_alloc_path(size_t name_len)
{
	init_mem_manager(&path_mem_manager, 0);
	return memory_pool_alloc(path_mem_manager, name_len);
}
