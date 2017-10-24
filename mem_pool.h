#ifndef MEM_POOL_H
#define MEM_POOL_H

struct mem_pool_manager {
	struct mem_pool *mem_pool;
	size_t alloc_size;
	size_t total_allocd;
};

struct mem_pool {
	struct mem_pool *next_pool;
	char *next_free;
	char *end;
	uintmax_t space[FLEX_ARRAY]; /* more */
};

void *mem_pool_alloc(struct mem_pool_manager *pool, size_t len);
void *mem_pool_calloc(struct mem_pool_manager *pool, size_t count, size_t size);

struct mem_pool *mem_pool_alloc_pool(struct mem_pool_manager *mem_pool_manager, size_t alloc_size);

int mem_pool_contains(struct mem_pool_manager *mem_pool_manager, void *mem);

#endif
