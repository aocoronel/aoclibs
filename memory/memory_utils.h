#ifndef MEMORY_UTILS
#define MEMORY_UTILS

#include <stddef.h>

/*
 * Copy n bytes from src to dest
*/
void *memory_copy(void *dest, const void *src, size_t n);

/*
 * Fill the first n bytes of the memory area pointed to by s with the constant byte c
*/
void *memory_set(void *s, int c, size_t n);

/*
 * Copy n bytes from src to dest, handling overlapping regions
*/
void *memory_move(void *dest, const void *src, size_t n);

#endif
