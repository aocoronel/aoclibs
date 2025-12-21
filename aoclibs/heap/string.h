#ifndef AOCLIBS_HEAP_STRING_H
#define AOCLIBS_HEAP_STRING_H

#include <aoclibs/common.h>
#include <aoclibs/heap/heap.h>

/*
 * Define heap allocated strings.
 * Enforces known length and memory size.
*/
typedef struct {
        Heap heap;
        usize length; // current length (not counting '\0')
} String;

// clang-format off

typedef struct { Err err; String value; } eString;

// clang-format on

eString werr_string(Err err, String s);

/*
 * Appends a string literal str to a String s.
 * May reallocate the String if necessary.
 *
 * Defines:
 * - NDEBUG :: Disable asserts
 * - DEBUG_HEAP :: Enable memory debugger
 *
 * Failure: errno << realloc
*/
Err string_append(String *_Nonnull s, const char *_Nonnull str, const usize str_len);

/*
 * Appends the contents of String s2 to String s1.
 * May reallocate s1 if necessary.
 *
 * Defines:
 * - NDEBUG :: Disable asserts
 * - DEBUG_HEAP :: Enable memory debugger
 *
 * Failure: errno << realloc
*/
Err string_string_append(String *_Nonnull s1, const String *_Nonnull s2);

/*
 * Clears a String's ending and reallocates to shrink memory.
 * A null-terminated character is set in the last whitespace.
 *
 * Defines:
 * - NDEBUG :: Disable asserts
 * - DEBUG_HEAP :: Enable memory debugger
 *
 * Failure: errno << realloc
*/
void string_clear(String *_Nonnull s);

/*
 * Clears the string content.
 * Sets a null-terminated character in the first index.
 *
 * Defines:
 * - NDEBUG :: Disable asserts
*/
void string_clear(String *_Nonnull s);

/*
 * Clears the string content.
 * Sets all characters to 0.
 *
 * Defines:
 * - NDEBUG :: Disable asserts
*/
void _string_garbage(String *_Nonnull s, usize len);

/*
 * equals a String s to a string literal.
 * Return Values:
 * < 0 - String s is tinier than str
 * 0 - String is equal to str
 * > 0 - String is bigger than str
 *
 * Defines:
 * - NDEBUG :: Disable asserts
*/
int string_equal(String *_Nonnull s, const char *_Nonnull str, const usize str_len);

/*
 * equals a String s1 to a String s2
 * Return Values:
 * < 0 - String s1 is tinier than String s2
 * 0 - String s1 is equal to String s2
 * > 0 - String s1 is bigger than String s2
 *
 * Defines:
 *
 * - NDEBUG :: Disable asserts
*/
int string_string_equal(const String *_Nonnull s1, const String *_Nonnull s2);

/*
 * Copies a string literal str to String s.
 * Replaces s1’s current contents entirely.
 * May reallocate s1 if necessary.
 *
 * Defines:
 * - NDEBUG :: Disable asserts
 * - DEBUG_HEAP :: Enable memory debugger
 *
 * Failure: errno << realloc
 */
Err _string_copy(String *_Nonnull s, const char *_Nonnull str, usize str_len);

/*
 * Copies the contents of String s2 into String s1.
 * Replaces s1’s current contents entirely.
 * May reallocate s1 if necessary.
 *
 * Defines:
 * - NDEBUG :: Disable asserts
 * - DEBUG_HEAP :: Enable memory debugger
 *
 * Failure: errno << realloc
 */
Err string_string_copy(String *_Nonnull s1, const String *_Nonnull s2);

/*
 * Allocates an empty String.
 *
 * If capacity is 0, sets capacity to 16.
 *
 * Defines:
 * - NDEBUG :: Disable asserts
 * - DEBUG_HEAP :: Enable memory debugger
 *
 * Failure: errno << malloc
 */
eString string_create(usize cap);

/*
 * Allocates a String from a string literal.
 *
 * The capacity is inferred by the string length.
 *
 * Defines:
 * - NDEBUG :: Disable asserts
 * - DEBUG_HEAP :: Enable memory debugger
 *
 * Failure: errno << malloc
 */
eString _string_from(const char *_Nonnull str, usize str_len);

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
 * - DEBUG_HEAP :: Enable memory debugger
 */
void string_free(String *s);

/*
 * Finds the first occurance of c in the String.
 *
 * Defines:
 * - NDEBUG :: Disable asserts
 *
 * Values:
 * - > 0 - returns the position of c.
 * - 0 - c hasn't been found.
 *
 * Failure: ErrValueNotFound
 */
eusize string_find(String *_Nonnull s, const char c);

/*
 * Finds the last occurance of c in the String.
 *
 * Defines:
 * - NDEBUG :: Disable asserts
 *
 * Values:
 * - > 0 - returns the position of c.
 * - 0 - c hasn't been found.
 *
 * Failure: ErrValueNotFound
 */
eusize string_reverse_find(String *_Nonnull s, const char c);

/*
 * Inserts a character into the String's ending.
 *
 * May reallocate the string if necessary.
 *
 * Defines:
 * - NDEBUG :: Disable asserts
 * - DEBUG_HEAP :: Enable memory debugger
 *
 * Failure: errno << realloc
 */
Err string_push(String *s, char c);

/*
 * Removes a character from the String's ending.
 *
 * Defines:
 * - NDEBUG :: Disable asserts
 *
 * Values:
 * - 0 - Last character is removed from the String.
 * - -1 - Removing failed. The String does not exist.
 *
 * Failure: ErrElementIsEmpty
 */
Err string_pop(String *s);

/*
 * Drops the last character from the String's ending, and resizes the string without reallocating.
 *
 * Defines:
 * - NDEBUG :: Disable asserts
 *
 * Values:
 * - 0 - Character is dropped from the String.
 * - -1 - Dropping failed. The String does not exist.
 *
 * Failure: ErrOutOfBounds
 */
Err string_drop(String *_Nonnull s, usize index);

/*
 * Reallocates the String to a new capacity.
 *
 * Defines:
 * - NDEBUG :: Disable asserts
 * - DEBUG_HEAP :: Enable memory debugger
 *
 * Failure: errno << realloc
 */
Err string_realloc(String *_Nonnull s, usize capacity);

/*
 * Clears a String's ending without reallocating.
 * A null-terminated character is set in the last whitespace.
 *
 * Defines:
 * - NDEBUG :: Disable asserts
*/
void string_trim_trailing(String *_Nonnull s);

/* === Macros === */

#define string_copy(s1, s2) _string_copy((s1), (s2), (sizeof(s2)))
#define string_from(s) _string_from(s, sizeof(s))
#define string_garbage(s) _string_garbage(s, s.length)

#endif // AOCLIBS_HEAP_STRING_H
