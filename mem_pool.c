/*
* This handles a pool of memory
*/

#include "cache.h"
#include "config.h"
#include "mem_pool.h"

void *mem_pool_alloc(struct mem_pool_manager *mem_pool_manager, size_t len)
{
	struct mem_pool *p;
	void *r;

	if (len == 0)
		len = 64;

	/* round up to a 'uintmax_t' alignment */
	if (len & (sizeof(uintmax_t) - 1))
		len += sizeof(uintmax_t) - (len & (sizeof(uintmax_t) - 1));

	for (p = mem_pool_manager->mem_pool; p; p = p->next_pool)
		if ((p->end - p->next_free >= len))
			break;

	if (!p) {
		if (len >= ((mem_pool_manager->alloc_size - sizeof(struct mem_pool)) / 2)) {
			// return xmalloc(len);
			p = mem_pool_alloc_pool(mem_pool_manager, len + sizeof(struct mem_pool));
		}
		else
			p = mem_pool_alloc_pool(mem_pool_manager, mem_pool_manager->alloc_size);

		// p = mem_pool_manager->mem_pool;
	}

	r = p->next_free;
	p->next_free += len;
	return r;
}

struct mem_pool *mem_pool_alloc_pool(struct mem_pool_manager *mem_pool_manager, size_t alloc_size)
{
	struct mem_pool *p;
	mem_pool_manager->total_allocd += alloc_size;

	trace_printf("mem_pool_alloc_pool size: %"PRIuMAX, (uintmax_t)alloc_size);

	// TODO: The pool needs to accomodate at least alloc_size content
	p = xmalloc(st_add(sizeof(struct mem_pool), alloc_size - sizeof(struct mem_pool)));

	p->next_pool = mem_pool_manager->mem_pool;
	p->next_free = (char *)p->space;
	p->end = p->next_free + alloc_size - sizeof(struct mem_pool);
	mem_pool_manager->mem_pool = p;

	return p;
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
