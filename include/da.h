#ifndef AOCLIBS_DA_H_
#define AOCLIBS_DA_H_

#include "base.h"

// This is implementation has few modifications and additions, and is inspired by
// the dynamic array available here: https://github.com/tsoding/nob.h

/*
 * Dynamic Array Implementation
 *
 * The whole implementation is done using macros, which allows to be used with any type the user
 * needs.
 *
 * Below is the a very generic dynamic arena struct. The data type can be swapped to something else
 * and it's going to work just like normal.
*/

// typedef struct {
//         size_t cap;
//         size_t len;
//         void *data; // must be a pointer!
// } DynamicArray;

#define AOCLIBS_DA_INITIAL_CAPACITY 256 // size in bytes allocated in the heap

#define AOCLIBS_DA_REALLOC realloc
#define AOCLIBS_DA_FREE free

// The following macros use the allocators defined above.
//
// Macros starting with "_" accept an extra argument: realloc
// Except for some like the very following ones, which are internal

// Convenient assertions to prevent access out of bounds
#define _aoc_assert_da_index_is_valid(da, index)                              \
        (ASSERT((da)->len > 0 && (da)->len < (da)->cap,                       \
                "invalid access to array at index %zu: len = %zu, cap = %zu", \
                (index),                                                      \
                (da)->len,                                                    \
                (da)->cap))
#define _aoc_assert_da_is_not_null(da) (ASSERT((da)->data != NULL, "cannot modify NULL pointer"))

// Zero initialize
#define aoc_da_init(da, new_cap)                                   \
        do {                                                       \
                (da)->data = calloc(new_cap, sizeof(*(da)->data)); \
                (da)->cap = new_cap;                               \
        } while (0)

// If you know before hand you need a big dynamic array, you can reserve memory.
//
// The new capacity is already calculates the size of the type.
//
// DynamicArray my_da = { 0 };
// da_reserve(&my_da, (&my_da)->len + 1); // Needs to allocate one value
#define aoc_da_reserve(da, new_cap)                                                              \
        do {                                                                                     \
                if ((new_cap) > (da)->cap) {                                                     \
                        if ((da)->cap < AOCLIBS_DA_INITIAL_CAPACITY) {                           \
                                (da)->cap = AOCLIBS_DA_INITIAL_CAPACITY;                         \
                        }                                                                        \
                        while ((new_cap) > (da)->cap) {                                          \
                                (da)->cap *= 2;                                                  \
                        }                                                                        \
                        (da)->data =                                                             \
                                AOCLIBS_DA_REALLOC((da)->data, (da)->cap * sizeof(*(da)->data)); \
                }                                                                                \
        } while (0)

#define aoc_da_free(da)                                    \
        do {                                               \
                ASSERT(da != NULL, "double free attempt"); \
                AOCLIBS_DA_FREE((da)->data);               \
                (da)->data = NULL;                         \
                (da)->len = 0;                             \
                (da)->cap = 0;                             \
        } while (0)

#define aoc_da_insert(da, item)                      \
        do {                                         \
                aoc_da_reserve((da), (da)->len + 1); \
                (da)->data[(da)->len++] = (item);    \
        } while (0)

#define aoc_da_append(da, items_buff, items_size) aoc_da_add(da, items_buff, items_size, (da)->len)

#define aoc_da_add(da, items_buff, items_size, offset)                                           \
        do {                                                                                     \
                aoc_da_reserve((da), (da)->len + (items_size));                                  \
                memcpy((da)->data + (offset), (items_buff), (items_size) * sizeof(*(da)->data)); \
                (da)->len += (items_size);                                                       \
        } while (0)

// =================================
// These macros skips the reserve step, for faster operations with less checks.
// Use at own risk.
#define aoc_da_insert_fast(da, item)              \
        do {                                      \
                (da)->data[(da)->len++] = (item); \
        } while (0)

#define aoc_da_append_fast(da, items_buff, items_size) \
        aoc_da_add_fast(da, items_buff, items_size, (da)->len)

#define aoc_da_add_fast(da, items_buff, items_size, offset)                                      \
        do {                                                                                     \
                memcpy((da)->data + (offset), (items_buff), (items_size) * sizeof(*(da)->data)); \
                (da)->len += (items_size);                                                       \
        } while (0)
// =================================

#define aoc_da_add_null(da) aoc_da_append(da, "\0", 1)

#define aoc_da_clone(dest, src)                                                 \
        do {                                                                    \
                aoc_da_reserve(dest, (src)->cap);                               \
                (dest)->len = (src)->len;                                       \
                memcpy((dest)->data, (src)->data, (src)->len * sizeof(void *)); \
        } while (0)

// Returns last pointer
#define aoc_da_last(da) (da)->data[_aoc_assert_da_index_is_valid(da, (da)->len - 1), (da)->len - 1]

// Returns last pointer, and removes it from the DA
#define aoc_da_pop(da) (da)->data[_aoc_assert_da_index_is_valid(da, (da)->len - 1), --(da)->len]

#define aoc_da_reset(da) (da)->len = 0

#define aoc_da_erase(da) memset((da)->data, 0, (da)->cap)

// Removes an item from the dynamic array from an index
#define aoc_da_drop(da, index)                                                              \
        do {                                                                                \
                _aoc_assert_da_is_not_null(da);                                             \
                ASSERT((da)->len > (index);     \
                memmove((da)->data[(index)], (da)->data[(index) + 1], (da)->len - (index)); \
                (da)->len--; \
                (da)->data[(da)->len] = 0;                                                  \
        } while (0)

// Faster da_drop
#define aoc_da_unordered_drop(da, i)                         \
        do {                                                 \
                size_t j = (i);                              \
                ASSERT(j < (da)->count);                     \
                (da)->items[j] = (da)->items[--(da)->count]; \
        } while (0)

// Swaps indexes "i1" and "i2"
#define aoc_da_swap(Type, da, i1, i2) swap(Type, (da)->data[i1], (da)->data[i2])

// Sorts using qsort and user-provided function "fn".
#define aoc_da_sort(da, fn) qsort((da)->data, (da)->len, sizeof((da)->data[0]), fn)

// Case a dynamic array is used with a buffer allocated in the stack, the following
// implementation can be used: Dynamic Array Stack (das)
//
// This is slighly different, because the dynamic array cannot be automatically resized.
// So let the user handle in case the append operation would truncate the buffer.
//
// The usage follows something like:
//
// if das_append(my_da, 1); // do nothing if fails
//
// OR
//
// if das_append(my_da, 1)
// else handle_error;
//
// These macros don't expect the da->data to be NULL.

#define aoc_das_insert(da, item)                  \
        ((da)->len + 1 < (da)->cap) {             \
                _aoc_assert_da_is_not_null(da);   \
                (da)->data[(da)->len++] = (item); \
        }

#define aoc_das_append(da, items_buff, items_size) \
        aoc_das_add(da, items_buff, items_size, (da)->len)

#define aoc_das_add(da, items_buff, items_size, offset)                                            \
        ((da)->len + items_size < (da)->cap) {                                                     \
                _aoc_assert_da_is_not_null(da);                                                    \
                memcpy((da)->data + (offset), (items_buff), (items_size) * (sizeof(*(da)->data))); \
                (da)->len += (items_size);                                                         \
        }

#define aoc_das_add_null(da) (da)->data[(da)->len] = '\0'

#define aoc_da_is_null(da) !(da) || !(da)->data

#ifdef AOCLIBS_STRIP_PREFIX
#define da_add aoc_da_add
#define da_copy aoc_da_append
#define da_insert aoc_da_insert
#define da_add_null aoc_da_add_null
#define da_clone aoc_da_clone
#define da_free aoc_da_free
#define da_reserve aoc_da_reserve

#define da_add_fast aoc_da_add_fast
#define da_copy_fast aoc_da_append_fast
#define da_insert_fast aoc_da_insert_fast

#define da_clear aoc_da_clear
#define da_drop aoc_da_drop
#define da_erase aoc_da_erase
#define da_last aoc_da_last
#define da_pop aoc_da_pop
#define da_sort aoc_da_sort
#define da_swap aoc_da_swap
#define da_unordered_drop aoc_da_unordered_drop

#define das_add aoc_das_add
#define das_copy aoc_das_append
#define das_add_null aoc_das_add_null
#define das_insert aoc_das_insert

#define da_is_null aoc_da_is_null
#endif

#endif
