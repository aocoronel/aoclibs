#ifndef AOCLIBS_DA_H_
#define AOCLIBS_DA_H_

#include <assert.h>

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
#define _aoc_assert_da_is_valid(da) (assert((da)->len > 0 && "out of bounds access"))

// Before any attempt to add new values to the DA, always reserve the memory.
//
// DynamicArray my_da = { 0 };
// da_reserve(&my_da, (&my_da)->len + 1); // Needs to allocate one value
#define aoc_da_reserve(da, new_cap) _aoc_da_reserve(AOCLIBS_DA_REALLOC, (da), (new_cap))
#define _aoc_da_reserve(realloc, da, new_cap)                                              \
        do {                                                                               \
                if ((new_cap) > (da)->cap) {                                               \
                        if ((da)->cap == 0) {                                              \
                                (da)->cap = AOCLIBS_DA_INITIAL_CAPACITY;                   \
                        }                                                                  \
                        while ((new_cap) > (da)->cap) {                                    \
                                (da)->cap *= 2;                                            \
                        }                                                                  \
                        (da)->data = realloc((da)->data, (da)->cap * sizeof(*(da)->data)); \
                }                                                                          \
        } while (0)

#define aoc_da_free(da) _aoc_da_free(AOCLIBS_DA_FREE, da)
#define _aoc_da_free(free, da)                             \
        do {                                               \
                ASSERT(da != NULL, "double free attempt"); \
                free((da)->data);                          \
                (da)->data = NULL;                         \
                (da)->len = 0;                             \
                (da)->cap = 0;                             \
        } while (0)

// Returns last pointer
#define aoc_da_last(da) (da)->data[_aoc_assert_da_is_valid(da), (da)->len - 1]

// Returns last pointer, and removes it from the DA
#define aoc_da_pop(da) \
        (da)->data[(assert((da)->len - 1 != SIZE_MAX && "out of bounds access")), --(da)->len]

// Empties the DA
#define aoc_da_clear(da) (da)->len = 0

// Append any type. For C strings, look for the aoc_da_append_many_cstr
#define aoc_da_append(da, data) _aoc_da_append(AOCLIBS_DA_REALLOC, (da), (data))
#define _aoc_da_append(realloc, da, item)                        \
        do {                                                     \
                _aoc_da_reserve((realloc), (da), (da)->len + 1); \
                (da)->data[(da)->len++] = (item);                \
        } while (0)

// This macro doesn't include the null terminator, and must be added manually
#define aoc_da_append_many_cstr(da, items_buff, items_size) \
        _aoc_da_append_many(AOCLIBS_DA_REALLOC, da, items_buff, cstrlen((items_buff), (items_size)))

// Adding strings using this macro will include the null terminator if the
// string buffer size is passed as items_size
#define aoc_da_append_many(da, items_buff, items_size) \
        _aoc_da_append_many(AOCLIBS_DA_REALLOC, da, items_buff, items_size)

#define _aoc_da_append_many(realloc, da, items_buff, items_size)                                  \
        do {                                                                                      \
                _aoc_da_reserve(realloc, (da), (da)->len + (items_size));                         \
                memcpy((da)->data + (da)->len, (items_buff), (items_size) * sizeof(*(da)->data)); \
                (da)->len += (items_size);                                                        \
        } while (0)

#define aoc_da_append_null(sb) _aoc_da_append_many(AOCLIBS_DA_REALLOC, sb, "", 1)
#define _aoc_da_append_null(realloc, sb) _aoc_da_append_many(realloc, sb, "", 1)

#define aoc_da_clone(dest, src) _aoc_da_clone(AOCLIBS_DA_REALLOC, dest, src)
#define _aoc_da_clone(realloc, dest, src)           \
        _aoc_da_reserve(realloc, dest, (src)->cap); \
        (dest)->len = (src)->len;                   \
        memcpy((dest)->data, (src)->data, (src)->len * sizeof(void *));

#define aoc_da_swap(Type, da, i1, i2) aoc_swap(Type, (da)->data[i1], (da)->data[i2])

#define aoc_da_sort(da, fn) qsort((da)->data, (da)->len, sizeof((da)->data[0]), fn)

#ifdef AOCLIBS_STRIP_PREFIX
#define _da_append _aoc_da_append
#define _da_append_many _aoc_da_append_many
#define _da_append_many_cstr _aoc_da_append_many_cstr
#define _da_append_null _aoc_da_append_null
#define _da_clone _aoc_da_clone
#define _da_free _aoc_da_free
#define _da_reserve _aoc_da_reserve

#define da_append aoc_da_append
#define da_append_many aoc_da_append_many
#define da_append_many_cstr aoc_da_append_many_cstr
#define da_append_null aoc_da_append_null
#define da_clone aoc_da_clone
#define da_free aoc_da_free
#define da_reserve aoc_da_reserve

#define da_clear aoc_da_clear
#define da_last aoc_da_last
#define da_pop aoc_da_pop
#define da_sort aoc_da_sort
#define da_swap aoc_da_swap
#endif

#endif
