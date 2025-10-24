#ifndef HEAP_ALLOC_H
#define HEAP_ALLOC_H

#include <stddef.h>

/*
 * gcc -DALLOC_DEBUG
 * Define ALLOC_DEBUG to enable runtime debugging
 */

/*
 * Defines a pointer used for heap allocation, which is followed by it's size.
 */
typedef struct HeapPtr {
    void *ptr;
    size_t mem; // current allocated size
} HeapPtr;

/*
 * Initialize a HeapPtr
 */
void heap_init(HeapPtr *h);

/*
 * High abstraction to malloc.
 * Heap allocate without initiliazing all values to 0.
 * Return values:
 * 0 - The HeapPtr does not exist, or malloc failed.
 * 1 - A new pointer and size are set to HeapPtr.
 */
int heap_alloc(HeapPtr *h, size_t size);

/*
 * High abstraction to calloc.
 * Heap allocate initiliazing all values to 0.
 * Return values:
 * 0 - The HeapPtr does not exist, or calloc failed.
 * 1 - A new pointer and size are set to HeapPtr.
 */
int heap_clean_alloc(HeapPtr *h, size_t count, size_t size);

/*
 * High abstraction to realloc.
 * Heap reallocate given HeapPtr to desired new size.
 * If realloc fails, the HeapPtr is untouched and still requires to be freed
 * with heap_free.
 * Return values:
 * 0 - The HeapPtr does not exist, or realloc failed.
 * 1 - A new pointer and size are set to HeapPtr.
*/
int resize_alloc(HeapPtr *h, size_t new_size);

/*
 * High abstraction to free.
 * Frees given HeapPtr.
 * Sets the pointer to NULL and set the memory to 0.
 */
void heap_free(HeapPtr *h);

#endif

// int main() {
//         HeapPtr h;
//         heap_init(&h);
//
//         if (!heap_alloc(&h, 16)) return 1;
//         strcpy((char *)h.ptr, "Hello");
//
//         if (!resize_alloc(&h, 32)) return 1;
//         strcat((char *)h.ptr, " World!");
//
//         printf("%s\n", (char *)h.ptr);
//
//         heap_free(&h);
//
//         return 0;
// }
