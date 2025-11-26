#ifndef AOCLIBS_STRING_STRCREATE_H
#define AOCLIBS_STRING_STRCREATE_H

#include <a_heap_main.h>
#include <a_error_main.h>
#include <a_string_types.h>

/*
 * Return an empty null terminated String.
 * Return Values:
 * String - return a String with a 16 bytes allocated in HeapPtr.ptr.
 * NULL - Allocattion fails.
*/
stringerr string_create(usize cap);

/*
 * Return a null terminated String from a string literal.
 * Return Values:
 * String - return a String in HeapPtr.ptr with a length-based bytes allocated.
 * NULL - Allocattion fails.
*/
stringerr string_from(const char *str);

/*
 * Frees heap allocated String and HeapPtr.ptr.
 * The memory and length are set to 0, and pointer is set to NULL.
*/
void string_free(String *s);

#endif // AOCLIBS_STRING_STRCREATE_H
