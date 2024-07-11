// SPDX-License-Identifier: BSD-3-Clause

#include <internal/mm/mem_list.h>
#include <internal/types.h>
#include <internal/essentials.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>

void *malloc(size_t size)
{
	void *result_addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (result_addr == MAP_FAILED) {
		return NULL;
	} else {
		int ver = mem_list_add(result_addr, size);
		if (ver == -1) {
			munmap(result_addr, size);
			return NULL;
		} else {
			return result_addr;
		}
	}
}

void *calloc(size_t nmemb, size_t size)
{
	void *result = malloc(nmemb * size);
	if (result == NULL) {
		return NULL;
	} else {
		char *iter = (char*) result;
		for (size_t i = 0; i < nmemb * size; ++i, ++iter) {
			*iter = 0;
		}
		return result;
	}
}

void free(void *ptr)
{
	struct mem_list *aux = mem_list_find(ptr);
	munmap(ptr, aux->len);
	mem_list_del(ptr);
}

void *realloc(void *ptr, size_t size)
{
	void *result = malloc(size);
	if (result == NULL) {
		return NULL;
	} else {
		struct mem_list *aux = mem_list_find(ptr);
		memcpy(result, ptr, aux->len);
		free(ptr);
		return result;
	}
}

void *reallocarray(void *ptr, size_t nmemb, size_t size)
{
	size_t result = nmemb * size;
	if (nmemb != 0 && result/nmemb != size)
		return NULL;
	return realloc(ptr, nmemb * size);
}
