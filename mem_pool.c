/*
* This handles a pool of memory
*/

#include "cache.h"
#include "config.h"
#include "mem_pool.h"

struct mp_block {
	struct mem_pool *next_block;
	char *next_free;
	char *end;
	uintmax_t space[FLEX_ARRAY]; /* more */
};

struct mp_block *mem_pool_alloc_block(struct mem_pool *pool, size_t alloc_size);

void mem_pool_init(struct mem_pool **mem_pool, size_t alloc_growth_size, size_t initial_size)
{
	if (!(*mem_pool))
	{
		// Set a minimum pool size
		if (alloc_growth_size < 1024 * 1024)
			alloc_growth_size = 1024 * 1024;

		*mem_pool = xmalloc(sizeof(struct mem_pool));
		(*mem_pool)->total_allocd = 0;
		(*mem_pool)->mp_block = 0;
		(*mem_pool)->alloc_size = alloc_growth_size;

		if (initial_size > 0)
			mem_pool_alloc_block((*mem_pool), initial_size);
	}
}

void *mem_pool_alloc(struct mem_pool *mem_pool, size_t len)
{
	struct mp_block *p;
	void *r;

	if (len == 0)
		len = 64;

	/* round up to a 'uintmax_t' alignment */
	if (len & (sizeof(uintmax_t) - 1))
		len += sizeof(uintmax_t) - (len & (sizeof(uintmax_t) - 1));

	for (p = mem_pool->mp_block; p; p = p->next_block)
		if ((p->end - p->next_free >= len))
			break;

	if (!p) {
		if (len >= ((mem_pool->alloc_size - sizeof(struct mem_pool)) / 2)) {
			p = mem_pool_alloc_block(mem_pool, len + sizeof(struct mp_block));
		}
		else
			p = mem_pool_alloc_block(mem_pool, mem_pool->alloc_size);
	}

	r = p->next_free;
	p->next_free += len;
	return r;
}

struct mp_block *mem_pool_alloc_block(struct mem_pool *mem_pool, size_t alloc_size)
{
	struct mp_block *p;
	mem_pool->total_allocd += alloc_size;

	trace_printf("mem_pool_alloc_pool size: %"PRIuMAX, (uintmax_t)alloc_size);

	// TODO: The pool needs to accomodate at least alloc_size content
	p = xmalloc(st_add(sizeof(struct mem_pool), alloc_size - sizeof(struct mp_block)));

	p->next_block = mem_pool->mp_block;
	p->next_free = (char *)p->space;
	p->end = p->next_free + alloc_size - sizeof(struct mp_block);
	mem_pool->mp_block = p;

	return p;
}


int mem_pool_contains(struct mem_pool *mem_pool, void *mem)
{
	struct mp_block *p;

	for (p = mem_pool->mp_block; p; p = p->next_block)
		if ((mem >= ((void *)p->space)) &&
		    (mem < ((void *)p->end)))
			return 1;

	return 0;
}

void *mem_pool_calloc(struct mem_pool *mem_pool, size_t count, size_t size)
{
	size_t len = count * size;
	void *r = mem_pool_alloc(mem_pool, len);
	memset(r, 0, len);
	return r;
}
