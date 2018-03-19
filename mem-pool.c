/*
 * Memory pool management.
 */

#include "cache.h"
#include "mem-pool.h"

#define MIN_ALLOC_GROWTH_SIZE 1024 * 1024

struct mp_block {
	struct mp_block *next_block;
	char *next_free;
	char *end;
	uintmax_t space[FLEX_ARRAY]; /* more */
};

static struct mp_block *mem_pool_alloc_block(struct mem_pool *mem_pool, size_t alloc_size)
{
	struct mp_block *p;
	mem_pool->total_allocd += alloc_size;

	p = xmalloc(st_add(sizeof(struct mp_block), alloc_size));

	p->next_block = mem_pool->mp_block;
	p->next_free = (char *)p->space;
	p->end = p->next_free + alloc_size;
	mem_pool->mp_block = p;

	return p;
}

void mem_pool_init(struct mem_pool **mem_pool, size_t alloc_growth_size, size_t initial_size)
{
	if (!(*mem_pool))
	{
		if (alloc_growth_size < MIN_ALLOC_GROWTH_SIZE)
			alloc_growth_size = MIN_ALLOC_GROWTH_SIZE;

		*mem_pool = xmalloc(sizeof(struct mem_pool));
		(*mem_pool)->total_allocd = 0;
		(*mem_pool)->mp_block = 0;
		(*mem_pool)->alloc_size = alloc_growth_size;

		if (initial_size > 0)
			mem_pool_alloc_block((*mem_pool), initial_size);
	}
}

void mem_pool_combine(struct mem_pool *dst, struct mem_pool *src)
{
	struct mp_block *p, *next_block;
	for (next_block = src->mp_block; next_block;) {
		p = next_block;
		next_block = next_block->next_block;
		p->next_block = dst->mp_block;
		dst->mp_block = p;
	}

	src->mp_block = NULL;
}

void mem_pool_discard(struct mem_pool *mem_pool)
{
	struct mp_block *block, *block_to_free;
	int invalidate_memory = should_validate_cache_entries();

	for (block = mem_pool->mp_block; block;)
	{
		block_to_free = block;
		block = block->next_block;

		if (invalidate_memory)
			memset(block_to_free->space, 0xCD, ((char *)block_to_free->end) - ((char *)block_to_free->space));

		free(block_to_free);
	}

	free(mem_pool);
}

int should_validate_cache_entries(void)
{
	static int validate_index_cache_entries = -1;

	if (validate_index_cache_entries < 0) {
		if (getenv("GIT_TEST_VALIDATE_INDEX_CACHE_ENTRIES"))
			validate_index_cache_entries = 1;
		else
			validate_index_cache_entries = 0;
	}

	return validate_index_cache_entries;
}

void *mem_pool_alloc(struct mem_pool *mem_pool, size_t len)
{
	struct mp_block *p;
	void *r;

	/* Round up to a 'uintmax_t' alignment */
	if (len & (sizeof(uintmax_t) - 1))
		len += sizeof(uintmax_t) - (len & (sizeof(uintmax_t) - 1));

	p = mem_pool->mp_block;

	if (p &&
	   (p->end - p->next_free < len)) {
		for (p = p->next_block; p; p = p->next_block)
			if (p->end - p->next_free >= len)
				break;
	}

	if (!p) {
		if (len >= ((mem_pool->alloc_size - sizeof(struct mp_block)) / 2)) {
			p = mem_pool_alloc_block(mem_pool, len);
		}
		else
			p = mem_pool_alloc_block(mem_pool, mem_pool->alloc_size);
	}

	r = p->next_free;
	p->next_free += len;
	return r;
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
