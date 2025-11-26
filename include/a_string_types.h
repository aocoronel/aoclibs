#ifndef AOCLIBS_STRING_TYPES_H
#define AOCLIBS_STRING_TYPES_H

#include <a_heap_types.h>
#include <a_error_types.h>

/*
 * Define heap allocated strings.
 * Enforces known length and memory size.
*/
typedef struct {
        HeapPtr heap;
        usize length; // current length (not counting '\0')
} String;

typedef struct {
        Error err;
        String value;
} stringerr;

#endif // AOCLIBS_STRING_TYPES_H
