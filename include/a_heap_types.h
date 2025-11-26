#ifndef AOCLIBS_HEAP_TYPES_H
#define AOCLIBS_HEAP_TYPES_H

#include <a_types.h>

/*
 * Defines a pointer used for heap allocation, which is followed by it's size.
 */
typedef struct HeapPtr {
    void *ptr;
    usize mem; // current allocated size
} HeapPtr;

#endif
