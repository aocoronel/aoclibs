#ifndef AOCLIBS_STRING_STRCLEAR_H
#define AOCLIBS_STRING_STRCLEAR_H

#include <a_heap_main.h>
#include <a_error_main.h>
#include <a_string_types.h>

/*
 * Clears a String's ending and reallocates to shrink memory.
 * A null-terminated character is set in the last whitespace.
 * Return Values:
 * 0 - String is flushed.
 * -1 - String does not exist, or reallocation failed.
*/
Error string_flush(String *s);

/*
 * Clears the string content.
 * Sets a null-terminated character in the first index.
*/
void string_clear(String *s);

/*
 * Clears the string content.
 * Sets all characters to 0.
*/
void string_garbage(void *ptr, usize len);

#endif // AOCLIBS_STRING_STRCLEAR_H
