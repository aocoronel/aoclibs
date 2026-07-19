#ifndef AOC_DA_H_
#define AOC_DA_H_

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

// For clarity, it's rather let the user create the struct, than make a macro that does it for them
#define DYNAMIC_ARRAY(type) \
	size_t cap;             \
	size_t len;             \
	type *data

// typedef struct {
//    DYNAMIC_ARRAY(void);
// } MyDynamicArray;

// Convenient assertions to prevent access out of bounds
#define _assert_da_index_is_valid(da, index)                               \
	($assert((da)->len > 0 && (da)->len < (da)->cap,                       \
			 "invalid access to array at index %zu: len = %zu, cap = %zu", \
			 (index),                                                      \
			 (da)->len,                                                    \
			 (da)->cap))
#define _assert_da_is_not_null(da) ($assert((da)->data != NULL, "cannot modify NULL pointer"))

// Zero initialize
#define da_init(da, new_cap)                               \
	do {                                                   \
		(da)->data = calloc(new_cap, sizeof(*(da)->data)); \
		(da)->cap = new_cap;                               \
	} while (0)

// If you know before hand you need a big dynamic array, you can reserve memory.
//
// The new capacity is already calculates the size of the type.
//
// DynamicArray my_da = { 0 };
// da_reserve(&my_da, (&my_da)->len + 1); // Needs to allocate one value
#define da_reserve_safe(da, new_cap)                                                     \
	({                                                                                   \
		void *ptr = _da_reserve((da)->data, &(da)->cap, (new_cap), sizeof(*(da)->data)); \
		$assert(ptr, "out of memory while reserving memory for dynamic array");          \
		(da)->data = (typeof((da)->data))ptr;                                            \
	})

#define da_reserve(da, new_cap)                                          \
	({                                                                   \
		(da)->data = (typeof((da)->data))_da_reserve(                    \
				(da)->data, &(da)->cap, (new_cap), sizeof(*(da)->data)); \
	})

AOCDEF void *_da_reserve(void *data, size_t *cap, size_t new_cap, const size_t sizeof_da);

#define da_free(da)                                 \
	do {                                            \
		$assert(da != NULL, "double free attempt"); \
		free((da)->data);                           \
		(da)->data = NULL;                          \
		(da)->len = 0;                              \
		(da)->cap = 0;                              \
	} while (0)

#define da_insert(da, item)                   \
	do {                                      \
		da_reserve_safe((da), (da)->len + 1); \
		(da)->data[(da)->len++] = (item);     \
	} while (0)

#define da_append(da, items_buff, items_size)          \
	do {                                               \
		da_add(da, items_buff, items_size, (da)->len); \
		(da)->len += (items_size);                     \
	} while (0)

#define da_add(da, items_buff, items_size, offset)                                       \
	do {                                                                                 \
		da_reserve_safe((da), (da)->len + (items_size));                                 \
		memcpy((da)->data + (offset), (items_buff), (items_size) * sizeof(*(da)->data)); \
	} while (0)

// =================================
// These macros skips the reserve step, for faster operations with less checks.
// Use at own risk.
#define da_insert_fast(da, item)          \
	do {                                  \
		(da)->data[(da)->len++] = (item); \
	} while (0)

#define da_append_fast(da, items_buff, items_size) \
	da_add_fast(da, items_buff, items_size, (da)->len)

#define da_add_fast(da, items_buff, items_size, offset)                                  \
	do {                                                                                 \
		memcpy((da)->data + (offset), (items_buff), (items_size) * sizeof(*(da)->data)); \
		(da)->len += (items_size);                                                       \
	} while (0)
// =================================

#define da_add_null(da) da_append(da, "\0", 1)

#define da_clone(dest, src)                                             \
	do {                                                                \
		da_reserve_safe(dest, (src)->cap);                              \
		(dest)->len = (src)->len;                                       \
		memcpy((dest)->data, (src)->data, (src)->len * sizeof(void *)); \
	} while (0)

// Returns last pointer
#define da_last(da) (da)->data[_assert_da_index_is_valid(da, (da)->len - 1), (da)->len - 1]

// Returns last pointer, and removes it from the DA
#define da_pop(da) (da)->data[_assert_da_index_is_valid(da, (da)->len - 1), --(da)->len]

#define da_reset(da) (da)->len = 0

#define da_erase(da) memset((da)->data, 0, (da)->cap)

// Removes an item from the dynamic array from an index
#define da_drop(da, index)                                                                      \
	do {                                                                                        \
		_assert_da_is_not_null(da);                                                             \
		$assert((da)->len > (index));                                                           \
		size_t remaining = (da)->len - (index) - 1;                                             \
		memmove(&(da)->data[index], &(da)->data[(index) + 1], remaining * sizeof(*(da)->data)); \
		(da)->len--;                                                                            \
	} while (0)

// Faster da_drop
#define da_unordered_drop(da, i)                     \
	do {                                             \
		$assert(i < (da)->count);                    \
		(da)->items[i] = (da)->items[--(da)->count]; \
	} while (0)

// Swaps indexes "i1" and "i2"
#define da_swap(da, i1, i2) swap((da)->data[i1], (da)->data[i2])

// Sorts using qsort and user-provided function "fn".
#define da_sort(da, fn) qsort((da)->data, (da)->len, sizeof((da)->data[0]), fn)

#define da_is_null(da) !(da) || !(da)->data

#ifdef AOC_IMPLEMENTATION
void *_da_reserve(void *data, size_t *cap, size_t new_cap, const size_t type_size) {
	size_t local_cap = *cap;
	if (new_cap <= local_cap) return data;
	if (local_cap < CONFIG_DA_DEFAULT_CAPACITY) {
		local_cap = CONFIG_DA_DEFAULT_CAPACITY;
	}
	while ((new_cap) > local_cap) {
		local_cap *= 2;
	}
	void *new_data = realloc(data, local_cap * type_size);
	*cap = local_cap;
	return new_data;
}
#endif

#endif
