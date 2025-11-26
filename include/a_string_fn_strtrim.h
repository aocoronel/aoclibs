#ifndef AOCLIBS_STRING_STRTRIM_H
#define AOCLIBS_STRING_STRTRIM_H

#include <a_heap_main.h>
#include <a_error_main.h>
#include <a_string_types.h>

/*
 * Clears a String's ending without reallocating.
 * A null-terminated character is set in the last whitespace.
*/
void string_trim_trailing(String *s);

#endif // AOCLIBS_STRING_STRTRIM_H
