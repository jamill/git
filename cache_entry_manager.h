#ifndef CACHE_ENTRY_MANAGER_H
#define CACHE_ENTRY_MANAGER_H

#include "mem_pool.h"

void *cache_entry_alloc(size_t len);
void set_cache_entry_size(size_t num_entries);
void cache_entry_free(struct cache_entry *ce);

#endif
