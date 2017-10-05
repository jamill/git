#ifndef MEM_POOL_H
#define MEM_POOL_H

struct mem_pool_manager {
	struct mem_pool *mem_pool;
	size_t alloc_size;
	size_t total_allocd;
};

void *memory_pool_alloc(struct mem_pool_manager *pool, size_t len);
void *memory_pool_calloc(struct mem_pool_manager *pool, size_t count, size_t size);

int memory_pool_contains(struct mem_pool_manager *mem_pool_manager, void *mem);

#endif
