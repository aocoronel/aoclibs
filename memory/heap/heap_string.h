#ifndef HEAP_STRING_H
#define HEAP_STRING_H

#include "heap_alloc.h"
#include <stddef.h>

#define strcat(s1, s2) string_append((s1), (s2))
#define sstrcat(s1, s2) string_string_append((s1), (s2))

#define strcmp(s1, s2) string_compare((s1), (s2))
#define sstrcmp(s1, s2) string_string_compare((s1), (s2))

#define strcpy(s1, s2) string_copy((s1), (s2))
#define sstrcpy(s1, s2) string_string_copy((s1), (s2))

#define strchr(s1, s2) string_find((s1), (s2))
#define strrchr(s1, s2) string_reverse_find((s1), (s2))

/*
 * Define heap allocated strings.
 * Enforces known length and memory size.
*/
typedef struct {
        HeapPtr heap; // heap_alloc.h:9
        size_t length; // current length (not counting '\0')
} String;

/*
 * Return an empty null terminated String.
 * Return Values:
 * String - return a String with a 16 bytes allocated in HeapPtr.ptr.
 * NULL - Allocattion fails.
*/
String *string_create(void);

/*
 * Return a null terminated String from a string literal.
 * Return Values:
 * String - return a String in HeapPtr.ptr with a length-based bytes allocated.
 * NULL - Allocattion fails.
*/
String *string_from(const char *str);

/*
 * Frees heap allocated String and HeapPtr.ptr.
 * The memory and length are set to 0, and pointer is set to NULL.
*/
void string_free(String *s);

/*
 * Appends a string literal str to a String s.
 * May reallocate the String if necessary.
 * Return Values:
 * 0 - the String has the string literal appended.
 * -1 - the String is untouched. String may be NULL, or allocation failed.
*/
int string_append(String *s, const char *str);

/*
 * Appends the contents of String s2 to String s1.
 * May reallocate s1 if necessary.
 * Return Values:
 *  0 - String s1 has String s2 appended.
 * -1 - the String is untouched. String may be NULL, or allocation failed.
 */
int string_string_append(String *s1, const String *s2);

/*
 * Copies a string literal str to String s.
 * Replaces s1’s current contents entirely.
 * May reallocate s1 if necessary.
 * Return Values:
 *  0 - s1 now contains the contents str
 * -1 - the String is untouched. String may be NULL, or allocation failed.
 */
int string_copy(String *s, const char *str);

/*
 * Copies the contents of String s2 into String s1.
 * Replaces s1’s current contents entirely.
 * May reallocate s1 if necessary.
 * Return Values:
 *  0 - s1 now contains the contents of s2
 * -1 - the String is untouched. String may be NULL, or allocation failed.
 */
int string_string_copy(String *s1, const String *s2);

/*
 * Finds the first occurance of c in the String.
 * Return Values:
 * > 0 - returns the position of c.
 * -1 - the String does not exist, or c hasn't been found.
*/
int string_find(String *s, const char c);

/*
 * Finds the last occurance of c in the String.
 * Return Values:
 * > 0 - returns the position of c.
 * -1 - the String does not exist, or c hasn't been found.
*/
int string_reverse_find(String *s, const char c);

/*
 * Compares a String s to a string literal.
 * Return Values:
 * < 0 - String s is tinier than str
 * 0 - String is equal to str
 * > 0 - String is bigger than str
*/
int string_compare(String *s, const char *str);

/*
 * Compares a String s1 to a String s2
 * Return Values:
 * < 0 - String s1 is tinier than String s2
 * 0 - String s1 is equal to String s2
 * > 0 - String s1 is bigger than String s2
*/
int string_string_compare(const String *s1, const String *s2);

/*
 * Clears a String's ending without reallocating.
 * A null-terminated character is set in the last whitespace.
*/
void string_trim_trailing(String *s);

/*
 * Clears a String's ending and reallocates to shrink memory.
 * A null-terminated character is set in the last whitespace.
 * Return Values:
 * 0 - String is flushed.
 * -1 - String does not exist, or reallocation failed.
*/
int string_flush(String *s);

/*
 * Inserts a character into the String's ending.
 * Return Values:
 * 0 - Character is inserted into the String.
 * -1 - Insertion failed. The String does not exist or reallocation failed.
*/
int string_push(String *s, char c);

/*
 * Removes a character from the String's ending.
 * Return Values:
 * 0 - Last character is removed from the String.
 * -1 - Removing failed. The String does not exist.
*/
int string_pop(String *s);

/*
 * Drops the last character from the String's ending, and resizes the string without reallocating.
 * Return Values:
 * 0 - Character is dropped from the String.
 * -1 - Dropping failed. The String does not exist.
*/
int string_drop(String *s, size_t index);

/*
 * Splits a String from a delimiter.
 * String out_s1 and out_s2 should not be initialized. Both strings are overwritten, and may cause
 * memory leaks.
 * Will allocate Strings in the heap and return to out_s1 and out_s2, potentially being NULL.
 * Check out_s1 and out_s2 to NULL to prevent dereferencing a NULL pointer.
 * Return Values:
 * 0 - Strings out_s1 and out_s2 may contain the split String s.
 * -1 - String does not exist.
*/
int string_split(String *s, char delimiter, String *out_s1, String *out_s2);

/*
 * Clears the string content.
 * Sets a null-terminated character in the first index.
*/
void string_clear(String *s);

/*
 * Clears the string content.
 * Sets all characters to 0.
*/
void string_garbage(void *ptr, size_t len);

#endif // !HEAP_STRING_H
