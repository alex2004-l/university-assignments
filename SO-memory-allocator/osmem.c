// SPDX-License-Identifier: BSD-3-Clause

#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>

#include "osmem.h"
#include "block_meta.h"

#define ALIGNMENT 8
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))
#define METADATA_SIZE (sizeof(struct block_meta))
#define MMAP_THRESHOLD (128 * 1024)

// Global variables
void *heap_start;

void merge_next_block(struct block_meta *current_block);
size_t get_block_size(struct block_meta *block);

// Function for heap preallocation
void *preallocate_heap(void)
{
	heap_start = sbrk(0);

	if (sbrk(MMAP_THRESHOLD) == (void *)-1)
		return NULL;

	struct block_meta *heap_block = (struct block_meta *) heap_start;

	heap_block->status = STATUS_FREE;
	heap_block->size = MMAP_THRESHOLD - ALIGN(METADATA_SIZE);
	heap_block->next = heap_block->prev = NULL;

	return heap_start;
}

// function that splits current block in two smaller blocks
void split_block(struct block_meta *current_block, size_t size)
{
	struct block_meta *new_block;

	size_t current_block_size = ALIGN(size) + ALIGN(METADATA_SIZE);
	size_t total_size = get_block_size(current_block);

	char *iter = (char *) current_block;

	iter += current_block_size;

	new_block = (struct block_meta *) iter;

	new_block->status = STATUS_FREE;
	new_block->next = current_block->next;
	new_block->prev = current_block;

	if (current_block->next != NULL)
		current_block->next->prev = new_block;

	merge_next_block(new_block);

	current_block->next = new_block;
	current_block->size = size;
	current_block->status = STATUS_ALLOC;

	new_block->size = get_block_size(new_block);
}


// function that requests more memory blocks
void *request_block(struct block_meta *last, size_t size)
{
	struct block_meta *new_block = sbrk(0);

	size_t new_block_size = ALIGN(METADATA_SIZE) + ALIGN(size);

	if (sbrk(new_block_size) == (void *) -1)
		return NULL;

	new_block->size = size;
	new_block->status = STATUS_ALLOC;
	new_block->prev = last;
	new_block->next = NULL;

	return new_block;
}

void *get_last_block(void)
{
	struct block_meta *iter, *last = NULL;

	iter = heap_start;
	if (heap_start == NULL)
		return NULL;


	while (iter != NULL) {
		last = iter;
		iter = iter->next;
	}

	return (void *) last;
}

size_t get_block_size(struct block_meta *block)
{
	if (block->next == NULL) {
		void *heap_end = sbrk(0);

		size_t result = (size_t) ((char *)heap_end - (char *) block) - ALIGN(METADATA_SIZE);
		return result;
	}

	size_t result = (size_t) ((char *)block->next - (char *) block) - ALIGN(METADATA_SIZE);
	return result;
}


void *alloc_via_mmap(size_t size, size_t total_size)
{
	void *mmap_result = mmap(NULL, total_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);

	if (mmap_result == MAP_FAILED)
		return NULL;


	// adding metadata info for new mapped memory
	struct block_meta *block = (struct block_meta *) mmap_result;

	block->status = STATUS_MAPPED;
	block->size = size;
	block->prev = block->next = NULL;

	return (void *)(block + 1);
}


// returns next block in list from start_point with size greater or equal to size parameter
void *get_next_fit(struct block_meta *start_point, size_t size)
{
	struct block_meta *iter = start_point;

	while (iter != NULL && !(iter->status == STATUS_FREE && get_block_size(iter) >= size))
		iter = iter->next;

	return (void *)iter;
}

void *get_best_fit(size_t size)
{
	struct block_meta *best_fit = NULL, *current;

	current = get_next_fit(heap_start, size);

	while (current != NULL) {
		if (best_fit == NULL || get_block_size(best_fit) > get_block_size(current))
			best_fit = current;
		current = get_next_fit(current->next, size);
	}

	if (best_fit != NULL) {
		if (get_block_size(best_fit) >= ALIGN(size) + ALIGN(METADATA_SIZE) + ALIGN(1)) {
			split_block(best_fit, size);
			return (void *) (best_fit + 1);
		}
		best_fit->status = STATUS_ALLOC;
		best_fit->size = size;
		return (void *) (best_fit + 1);
	}
	struct block_meta *last = get_last_block();

	if (last->status == STATUS_FREE) {
		size_t additional_size = ALIGN(size - last->size);

		if (sbrk(additional_size) == (void *) -1)
			return NULL;

		last->size = size;
		last->status = STATUS_ALLOC;

		return (void *)(last + 1);
	}
	best_fit = request_block(last, size);
	if (best_fit == NULL)
		return NULL;

	last->next = best_fit;

	return (void *) (best_fit + 1);
}


/************************ IMPLEMENTATION OF OS_MALLOC ************************/
void *os_malloc(size_t size)
{
	// invalid size for allocation
	if (size <= 0)
		return NULL;

	size_t total_size = ALIGN(METADATA_SIZE) + ALIGN(size);

	if (total_size > MMAP_THRESHOLD)
		return alloc_via_mmap(size, total_size);
		// verifying if heap was preallocated
	if (heap_start == NULL) {
		if (preallocate_heap() == NULL)
			return NULL;
	}
	return get_best_fit(size);
}


void merge_next_block(struct block_meta *current_block)
{
	while (current_block->next != NULL && current_block->next->status == STATUS_FREE) {
		struct block_meta *next_block = current_block->next->next;

		if (next_block == NULL) {
			void *heap_end = sbrk(0);

			current_block->size = (size_t) ((char *) heap_end - (char *) current_block) - ALIGN(METADATA_SIZE);
			current_block->next = NULL;
		} else {
			current_block->size = (size_t) ((char *) next_block - (char *) current_block) - ALIGN(METADATA_SIZE);
			next_block->prev = current_block;
			current_block->next = next_block;
		}
	}
}


void merge_prev_block(struct block_meta *current_block)
{
	while (current_block->prev != NULL && current_block->prev->status == STATUS_FREE) {
		struct block_meta *prev_block = current_block->prev;
		struct block_meta *next_block = current_block->next;

		if (next_block == NULL) {
			void *heap_end = sbrk(0);

			prev_block->size = (size_t) ((char *)heap_end - (char *)prev_block) - ALIGN(METADATA_SIZE);
			prev_block->next = NULL;
		} else {
			prev_block->size = (size_t) ((char *)next_block - (char *)prev_block) - ALIGN(METADATA_SIZE);
			prev_block->next = next_block;
			next_block->prev = prev_block;
		}
		current_block = prev_block;
	}
}

/************************ IMPLEMENTATION OF OS_FREE ************************/
void os_free(void *ptr)
{
	if (ptr == NULL)
		return;

	struct block_meta *block = ((struct block_meta *)ptr) - 1;

	if (block->status == STATUS_MAPPED) {
		int munmap_result = munmap(block, ALIGN(block->size) + ALIGN(METADATA_SIZE));
	} else if (block->status == STATUS_ALLOC) {
		block->status = STATUS_FREE;
		merge_next_block(block);
		merge_prev_block(block);
		block->size = get_block_size(block);
	} else if (block->status == STATUS_FREE) {
		block->size = get_block_size(block);
	}
}

/* IMPLEMENTATION OF OS_CALLOC */
void *os_calloc(size_t nmemb, size_t size)
{
	size_t result = nmemb * size;
	//if (result/size != nmemb)
	//	return NULL;

	if (nmemb == 0 || size == 0)
		return NULL;

	void *ptr;
	size_t page_size = getpagesize();

	if (ALIGN(result) + ALIGN(METADATA_SIZE) > page_size) {
		ptr =  alloc_via_mmap(result, ALIGN(result) + ALIGN(METADATA_SIZE));
	} else {
		if (heap_start == NULL) {
			if (preallocate_heap() == NULL)
				return NULL;
		}
		ptr = get_best_fit(result);
	}
	if (ptr == NULL)
		return NULL;

	struct block_meta *metadata = ptr;

	metadata--;
	memset(ptr, 0, metadata->size);
	return ptr;
}

/* IMPLEMENTATION OF OS_REALLOC */
void *os_realloc(void *ptr, size_t size)
{
	if (ptr == NULL && size == 0)
		return NULL;

	if (ptr == NULL)
		return os_malloc(size);

	if (size == 0) {
		os_free(ptr);
		return NULL;
	}

	// getting metadata block
	struct block_meta *block = ptr;

	block--;
	// when reallocing a block mapped with mmap
	if (block->status == STATUS_MAPPED) {
		// allocating on heap if size is smaller than mmap_threshold
		if (ALIGN(METADATA_SIZE) + ALIGN(size) <= MMAP_THRESHOLD) {
			if (heap_start == NULL) {
				if (preallocate_heap() == NULL)
					return ptr;
			}

			void *new_ptr = get_best_fit(size);
			size_t min_val = (block->size > size) ? size : block->size;

			memcpy(new_ptr, ptr, min_val);
			os_free(ptr);

			return new_ptr;
		}
		// calling mmap and mapping a new memory region if size is greater than mmap_threshold
		void *new_ptr = alloc_via_mmap(size, ALIGN(METADATA_SIZE) + ALIGN(size));
		size_t min_val = (block->size > size) ? size : block->size;

		memcpy(new_ptr, ptr, min_val);
		os_free(ptr);

		return new_ptr;
	}

	if (block->status == STATUS_ALLOC) {
		if (ALIGN(METADATA_SIZE) + ALIGN(size) > MMAP_THRESHOLD) {
			void *new_ptr = alloc_via_mmap(size, ALIGN(METADATA_SIZE) + ALIGN(size));
			size_t min_val = (block->size > size) ? size : block->size;

			memcpy(new_ptr, ptr, min_val);
			os_free(ptr);

			return new_ptr;
		}
		if (get_block_size(block) >= ALIGN(size) + ALIGN(METADATA_SIZE) + ALIGN(1)) {
			split_block(block, size);
			return (void *) ptr;
		}
		if (get_block_size(block) >= ALIGN(size)) {
			block->size = size;
			return (void *) ptr;
		}

		if (block->next == NULL) {
			size_t additional_size = ALIGN(size) - ALIGN(block->size);

			if (sbrk(additional_size) == (void *) -1)
				return ptr;

			block->size = size;

			return (void *) ptr;
		}

		if (block->next->status == STATUS_FREE) {
			merge_next_block(block->next);

			size_t free_size = get_block_size(block->next) + ALIGN(METADATA_SIZE);
			size_t block_size = get_block_size(block);

			if (free_size >= ALIGN(size) - block_size) {
				if (block->next->next != NULL)
					block->next->next->prev = block;

				block->next = block->next->next;
				if (free_size + block_size >= ALIGN(size) + ALIGN(METADATA_SIZE) + ALIGN(1))
					split_block(block, size);

				return (void *)ptr;
			}
		}

		void *ptr_result = get_best_fit(size);

		if (ptr_result == NULL)
			return ptr;

		memcpy(ptr_result, ptr, size);
		os_free(ptr);

		return ptr_result;
	}
	return NULL;
}
