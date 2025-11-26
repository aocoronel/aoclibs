#ifndef AOCLIBS_STRING_STRMOV_H
#define AOCLIBS_STRING_STRMOV_H

#include <a_heap_main.h>
#include <a_error_main.h>
#include <a_string_types.h>

/*
 * Inserts a character into the String's ending.
 * Return Values:
 * 0 - Character is inserted into the String.
 * -1 - Insertion failed. The String does not exist or reallocation failed.
*/
Error string_push(String *s, char c);

/*
 * Removes a character from the String's ending.
 * Return Values:
 * 0 - Last character is removed from the String.
 * -1 - Removing failed. The String does not exist.
*/
Error string_pop(String *s);

/*
 * Drops the last character from the String's ending, and resizes the string without reallocating.
 * Return Values:
 * 0 - Character is dropped from the String.
 * -1 - Dropping failed. The String does not exist.
*/
Error string_drop(String *s, usize index);

#endif // AOCLIBS_STRING_STRMOV_H
