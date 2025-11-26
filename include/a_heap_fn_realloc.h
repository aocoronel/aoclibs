#ifndef AOCLIBS_HEAP_REALLOC_H
#define AOCLIBS_HEAP_REALLOC_H

#include <a_heap_types.h>
#include <a_error_types.h>

/*
 * High abstraction to realloc.
 * Heap reallocate given HeapPtr to desired new size.
 * If realloc fails, the HeapPtr is untouched and still requires to be freed
 * with heap_free.
 * Return values:
 * 0 - The HeapPtr does not exist, or realloc failed.
 * 1 - A new pointer and size are set to HeapPtr.
*/
Error resize_alloc(HeapPtr *h, usize new_size);

#endif
