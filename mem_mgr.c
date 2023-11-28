#include "mem_mgr.h"

#include <stdio.h>
#include <stdlib.h>

#define BLOCK_ARR_SIZE 1000
#define FOR_EACH_BLOCK(block) \
	for ((block) = block_arr; (block) < block_arr + BLOCK_ARR_SIZE; ++(block))

struct block {
	const void *ptr;
	size_t size;
};

static struct block block_arr[BLOCK_ARR_SIZE];
static size_t block_arr_count;

static struct block *ptr_block_get(const void *ptr) {
	struct block *ret;

	FOR_EACH_BLOCK(ret)
		if (ret->ptr == ptr)
			return ret;

	return NULL;
}

static struct block *avail_block_get(void) {
	return block_arr_count < BLOCK_ARR_SIZE ? ptr_block_get(NULL) : NULL;
}

static void block_set(const void *ptr, size_t size) {
	struct block *block = avail_block_get();
	if (block != NULL) {
		block->ptr = ptr;
		block->size = size;
		++block_arr_count;
	}
}

static void block_modify(const void *old_ptr, const void *new_ptr,
						 size_t new_size) {
	struct block *block = ptr_block_get(old_ptr);
	if (block != NULL) {
		block->ptr = new_ptr;
		block->size = new_size;
	} else {
		block_set(new_ptr, new_size);
	}
}

static void block_clear(const void *ptr) {
	struct block *block = ptr_block_get(ptr);
	if (block != NULL) {
		block->ptr = NULL;
		block->size = 0;
		--block_arr_count;
	}
}

void *mem_mgr_calloc(size_t num, size_t size) {
	void *ret;

	ret = calloc(num, size);
	if (ret != NULL)
		block_set(ret, num * size);

	return ret;
}

void *mem_mgr_malloc(size_t size) {
	void *ret;

	ret = malloc(size);
	if (ret != NULL)
		block_set(ret, size);

	return ret;
}

void *mem_mgr_realloc(void *ptr, size_t size) {
	void *ret;

	ret = realloc(ptr, size);
	if (size == 0)
		block_clear(ptr);
	else if (ret != NULL)
		block_modify(ptr, ret, size);

	return ret;
}


void mem_mgr_free(void *ptr) {
	block_clear(ptr);
	free(ptr);
}

size_t mem_mgr_size_get(void) {
	size_t ret = 0;
	struct block *block;

	if (block_arr_count == 0)
		return 0;

	FOR_EACH_BLOCK(block)
		ret += block->size;

	return ret;
}

void mem_mgr_list_print(void) {
	struct block *block;
	size_t total = 0;

	puts("|===========================================|");
	puts("|     Address      |          Size          |");

	if (block_arr_count != 0) {
		puts("|-------------------------------------------|");
		FOR_EACH_BLOCK(block)
			if (block->ptr != NULL) {
				printf("| %16p | %20u B |\n", block->ptr, block->size);
				total += block->size;
			}
	}

	puts("|-------------------------------------------|");
	printf("|      Total       | %20u B |\n", total);
	puts("|===========================================|");
}
