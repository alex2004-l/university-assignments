// SPDX-License-Identifier: BSD-3-Clause

#include <string.h>

#define BUFFER_SIZE 1024


char *strcpy(char *destination, const char *source)
{
	if(destination != NULL) {
		char *idx;
		for(idx = destination; *source != '\0'; ++source, ++idx) {
			*idx = *source;
		}
		*idx = '\0';
	}
	return destination;
}


char *strncpy(char *destination, const char *source, size_t len)
{
	if(destination != NULL) {
		char *str_idx = destination;
		size_t idx = 0;
		for ( ; idx < len; ++idx, ++str_idx) {
			*str_idx = *source;
			if (*source == '\0') {
				break;
			} else {
				++source;
			}
		}
	}
	return destination;
}


char *strcat(char *destination, const char *source)
{
	if(destination != NULL) {
		strcpy(destination + strlen(destination), source);
	}
	return destination;
}


char *strncat(char *destination, const char *source, size_t len)
{
	if (destination != NULL) {
		char *str_idx = destination + strlen(destination);
		size_t idx = 0;
		for ( ; idx < len; ++idx, ++str_idx) {
			if (*source == '\0') {
				break;
			} else {
				*str_idx = *source;
				++source;
			}
		}
		*str_idx = '\0';
	}
	return destination;
}


int strcmp(const char *str1, const char *str2)
{
	for(; *str1 != '\0' && *str2 != '\0'; str1++, str2++) {
		if (*str1 == *str2) {
			continue;
		} else {
			if (*str1 - *str2 > 0)
				return 1;
			else
				return -1;
		}
	}
	if (*str1 == *str2)
		return 0;
	return (*str2 == '\0')? 1 : -1;
}


int strncmp(const char *str1, const char *str2, size_t len)
{
	size_t idx = 0;
	for (; idx < len; str1++, str2++, idx++) {
		if(*str1 == *str2)
			continue;
		if (*str1 == '\0' || *str1 - *str2 < 0)
			return -1;
		else
			return 1;
	}
	return 0;
}


size_t strlen(const char *str)
{
	size_t i = 0;

	for (; *str != '\0'; str++, i++)
		;

	return i;
}


char *strchr(const char *str, int c)
{
	if (str != NULL) {
		for (; *str != '\0'; ++str) {
			if (*str == c)
				return (char *)str;
		}
	}
	return NULL;
}


char *strrchr(const char *str, int c)
{
	char *aux = NULL;
	if (str != NULL) {
		for(; *str != '\0'; str ++) {
			if (*str == c)
				aux = (char *)str;
		}
	}
	return aux;
}


char *strstr(const char *haystack, const char *needle)
{
	for (; *haystack != '\0'; haystack ++) {
		const char *idx = haystack;
		const char *needle_idx = needle;
		for (;*needle_idx != '\0'; ++needle_idx, ++idx) {
			if (*needle_idx != *idx)
				break;
		}
		if(*needle_idx == '\0')
			return (char *)haystack;
	}
	return NULL;
}


char *strrstr(const char *haystack, const char *needle)
{
	char *tmp = NULL;
	for (; *haystack != '\0'; haystack ++) {
		const char *idx = haystack;
		const char *needle_idx = needle;
		for (;*needle_idx != '\0'; ++needle_idx, ++idx) {
			if (*needle_idx != *idx)
				break;
		}
		if(*needle_idx == '\0')
			tmp = (char *) haystack;
	}
	return tmp;
}


void *memcpy(void *destination, const void *source, size_t num)
{
	strncpy(destination, source, num);
	return destination;
}


void *memmove(void *destination, const void *source, size_t num)
{
	char buffer[BUFFER_SIZE];
	while (num > BUFFER_SIZE) {
		strncpy(buffer, source, BUFFER_SIZE);
		strncpy(destination, buffer, BUFFER_SIZE);
		num = num - BUFFER_SIZE;
	}
	strncpy(buffer, source, num);
	strncpy(destination, buffer, num);
	return destination;
}


int memcmp(const void *ptr1, const void *ptr2, size_t num)
{
	return strncmp(ptr1, ptr2, num);
}


void *memset(void *source, int value, size_t num)
{
	char *aux = (char *) source;
	for (size_t idx = 0; idx < num; ++idx, ++aux) {
		*aux = value;
	}
	return source;
}
