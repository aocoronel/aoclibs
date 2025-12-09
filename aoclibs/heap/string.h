#ifndef AOCLIBS_HEAP_STRING_H
#define AOCLIBS_HEAP_STRING_H

#include <aoclibs/heap/heap.h>
#include <aoclibs/error.h>
#include <aoclibs/int.h>

/*
 * Define heap allocated strings.
 * Enforces known length and memory size.
*/
typedef struct {
        Heap heap;
        usize length; // current length (not counting '\0')
} String;

// clang-format off

typedef struct { Err err; String value; } string_e;

// clang-format on

/*
 * Appends a string literal str to a String s.
 * May reallocate the String if necessary.
 *
 * Defines:
 *
 * - NDEBUG: Disable debug prints. Disable non null ptr assert.
 * - DEBUG_HEAP: Enable memory debugger
 *
 * Err.codes:
 * - Success: ok()
 * - Fail: errno
*/
Err string_append(String *s, const char *str, usize str_len);

/*
 * Appends the contents of String s2 to String s1.
 * May reallocate s1 if necessary.
 *
 * Defines:
 *
 * - NDEBUG: Disable debug prints. Disable non null ptr assert.
 * - DEBUG_HEAP: Enable memory debugger
 *
 * Err.codes:
 * - Success: ok()
 * - Fail: errno
*/
Err string_string_append(String *s1, const String *s2);

/*
 * Clears a String's ending and reallocates to shrink memory.
 * A null-terminated character is set in the last whitespace.
 *
 * Defines:
 *
 * - NDEBUG: Disable debug prints. Disable non null ptr assert.
 * - DEBUG_HEAP: Enable memory debugger
 *
 * Err.codes:
 * - Success: ok()
 * - Fail: errno
*/
Err string_flush(String *s);

/*
 * Clears the string content.
 * Sets a null-terminated character in the first index.
 *
 * Defines:
 *
 * - NDEBUG: Disable non null s assert.
*/
void string_clear(String *s);

/*
 * Clears the string content.
 * Sets all characters to 0.
 *
 * Defines:
 *
 * - NDEBUG: Disable non null ptr assert.
*/
void _string_garbage(String *s, usize len);

/*
 * Compares a String s to a string literal.
 * Return Values:
 * < 0 - String s is tinier than str
 * 0 - String is equal to str
 * > 0 - String is bigger than str
 *
 * Defines:
 *
 * - NDEBUG: Disable non null s and str assert.
*/
int string_compare(String *s, const char *str);

/*
 * Compares a String s1 to a String s2
 * Return Values:
 * < 0 - String s1 is tinier than String s2
 * 0 - String s1 is equal to String s2
 * > 0 - String s1 is bigger than String s2
 *
 * Defines:
 *
 * - NDEBUG: Disable non null s1 and s2 assert.
*/
int string_string_compare(const String *s1, const String *s2);

/*
 * Copies a string literal str to String s.
 * Replaces s1’s current contents entirely.
 * May reallocate s1 if necessary.
 *
 * Defines:
 *
 * - NDEBUG: Disable debug prints. Disable non null s and str assert.
 * - DEBUG_HEAP: Enable memory debugger
 *
 * Err.code:
 *
 * - Success: ok()
 *
 * - Fail: errno
 */
Err _string_copy(String *s, const char *str, usize str_len);

/*
 * Copies the contents of String s2 into String s1.
 * Replaces s1’s current contents entirely.
 * May reallocate s1 if necessary.
 *
 * Defines:
 *
 * - NDEBUG: Disable debug prints. Disable non null s1 and s2 assert.
 * - DEBUG_HEAP: Enable memory debugger
 *
 * Err.code:
 *
 * - Success: ok()
 *
 * - Fail: errno
 */
Err string_string_copy(String *s1, const String *s2);

/*
 * Allocates an empty String.
 *
 * If capacity is 0, sets capacity to 16.
 *
 * Defines:
 *
 * - NDEBUG: Disable debug prints.
 * - DEBUG_HEAP: Enable memory debugger
 *
 * Err.code:
 *
 * - Success: ok()
 *
 * - Fail: errno
 */
string_e string_create(usize cap);

/*
 * Allocates a String from a string literal.
 *
 * The capacity is inferred by the string length.
 *
 * Defines:
 *
 * - NDEBUG: Disable debug prints. Disable non null str assert.
 * - DEBUG_HEAP: Enable memory debugger
 *
 * Err.code:
 *
 * - Success: ok()
 *
 * - Fail: errno
 */
string_e _string_from(const char *str, usize len);

/*
 * Frees heap allocated String and HeapPtr.ptr.
 *
 * Frees given HeapPtr.
 *
 * The memory and length are set to 0, and pointer is set to NULL.
 *
 * If h or h.ptr is NULL, returns earlier
 *
 * Defines:
 *
 * - DEBUG_HEAP: Enable memory debugger
 */
void string_free(String *s);

/*
 * Finds the first occurance of c in the String.
 *
 * Defines:
 *
 * - NDEBUG: Disable non null s assert.
 *
 * Values:
 *
 * - > 0 - returns the position of c.
 *
 * - 0 - c hasn't been found.
 *
 * Err.code:
 *
 * - Success: ok()
 *
 * - Fail: ERROR_FAIL
 */
usize_e string_find(String *s, const char c);

/*
 * Finds the last occurance of c in the String.
 *
 * Defines:
 *
 * - NDEBUG: Disable non null s assert.
 *
 * Values:
 *
 * - > 0 - returns the position of c.
 *
 * - 0 - c hasn't been found.
 *
 * Err.code:
 *
 * - Success: ok()
 *
 * - Fail: ERROR_FAIL
 */
usize_e string_reverse_find(String *s, const char c);

/*
 * Inserts a character into the String's ending.
 *
 * May reallocate the string if necessary.
 *
 * Defines:
 *
 * - NDEBUG: Disable debug prints. Disable non null s and str assert.
 * - DEBUG_HEAP: Enable memory debugger
 *
 * Err.code:
 *
 * - Success: ok()
 *
 * - Fail: errno
 */
Err string_push(String *s, char c);

/*
 * Removes a character from the String's ending.
 *
 * Values:
 * - 0 - Last character is removed from the String.
 * - -1 - Removing failed. The String does not exist.
 *
 * Err.code:
 *
 * - Success: ok()
 *
 * - Fail: ERROR_FAIL
 */
Err string_pop(String *s);

/*
 * Drops the last character from the String's ending, and resizes the string without reallocating.
 *
 * Values:
 * - 0 - Character is dropped from the String.
 * - -1 - Dropping failed. The String does not exist.
 *
 * Err.code:
 *
 * - Success: ok()
 *
 * - Fail: ERROR_FAIL
 */
Err string_drop(String *s, usize index);

/*
 * Reallocates the String to a new capacity.
 *
 * Defines:
 *
 * - NDEBUG: Disable debug prints. Disable non null h assert.
 * - DEBUG_HEAP: Enable memory debugger
 *
 * Err.code:
 *
 * - Success: ok()
 *
 * - Fail: errno
 */
Err string_realloc(String *s, usize capacity);

/*
 * Splits a String from in the first occurance of a delimiter.
 *
 * out_s1 and out_s2 will allocate and are mutated to contain the split string
 *
 * Defines:
 *
 * - NDEBUG: Disable debug prints. Disable non null ptr assert.
 * - DEBUG_HEAP: Enable memory debugger
 *
 * Err.codes:
 * - Success: ok()
 * - Fail: errno
*/
Err string_split(String *s, char delimiter, String *out_s1, String *out_s2);

/*
 * Splits a String from in the last occurance of a delimiter.
 *
 * out_s1 and out_s2 will allocate and are mutated to contain the split string
 *
 * Defines:
 *
 * - NDEBUG: Disable debug prints. Disable non null ptr assert.
 * - DEBUG_HEAP: Enable memory debugger
 *
 * Err.codes:
 * - Success: ok()
 * - Fail: errno
*/
Err string_rsplit(String *s, char delimiter, String *out_s1, String *out_s2);

/*
 * Clears a String's ending without reallocating.
 * A null-terminated character is set in the last whitespace.
*/
void string_trim_trailing(String *s);

/* === Macros === */

#define a_string_copy(s1, s2) string_copy((s1), (s2), (sizeof(s2)))
#define a_string_from(s) _string_from(s, sizeof(s))
#define a_string_garbage(s) _string_garbage(s, s.length)

#endif // AOCLIBS_HEAP_STRING_H
