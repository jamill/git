/*
* This handles a pool of string
*/

#include "cache.h"
#include "config.h"
#include "mem_pool.h"

struct mem_pool {
	struct mem_pool *next_pool;
	char *next_free;
	char *end;
	uintmax_t space[FLEX_ARRAY]; /* more */
};

void *mem_pool_alloc(struct mem_pool_manager *mem_pool_manager, size_t len)
{
	struct mem_pool *p;
	void *r;

	if (len == 0)
		len = 1024 * 1024;

	/* round up to a 'uintmax_t' alignment */
	if (len & (sizeof(uintmax_t) - 1))
		len += sizeof(uintmax_t) - (len & (sizeof(uintmax_t) - 1));

	for (p = mem_pool_manager->mem_pool; p; p = p->next_pool)
		if ((p->end - p->next_free >= len))
			break;

	if (!p) {
		if (len >= ((mem_pool_manager->alloc_size - sizeof(struct mem_pool)) / 2)) {
			mem_pool_manager->total_allocd += len;
			return xmalloc(len);
		}

		mem_pool_manager->total_allocd += mem_pool_manager->alloc_size;
		p = xmalloc(st_add(sizeof(struct mem_pool), mem_pool_manager->alloc_size - sizeof(struct mem_pool)));
		p->next_pool = mem_pool_manager->mem_pool;
		p->next_free = (char *)p->space;
		p->end = p->next_free + mem_pool_manager->alloc_size - sizeof(struct mem_pool);
		mem_pool_manager->mem_pool = p;
	}

	r = p->next_free;
	p->next_free += len;
	return r;
}

int mem_pool_contains(struct mem_pool_manager *mem_pool_manager, void *mem)
{
	struct mem_pool *p;

	for (p = mem_pool_manager->mem_pool; p; p = p->next_pool)
		if ((mem >= ((void *)p->space)) &&
		    (mem < ((void *)p->end)))
			return 1;

	return 0;
}

void *mem_pool_calloc(struct mem_pool_manager *mem_pool_manager, size_t count, size_t size)
{
	size_t len = count * size;
	void *r = mem_pool_alloc(mem_pool_manager, len);
	memset(r, 0, len);
	return r;
}
