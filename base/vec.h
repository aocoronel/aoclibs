#ifndef AOCLIBS_VEC_H_
#define AOCLIBS_VEC_H_

#include "attributes.h"
#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef int (*VecCompare)(const void *a, const void *b);

typedef struct {
        void *data;
        size_t len;
        size_t cap;
        size_t pad; // Size of each element in bytes
} Vec;

#define AOCLIBS_VECTOR_GROWTH_FACTOR 2

/*
 * Initialize a new vector with corresponding padding.
 *
 * The padding refers to the size of the type you want to put in the vector.
 *
 * The vector allocates a single time and grows overtime, and can only store data with the same
 * padding.
*/
Vec aoc_vec_init(size_t pad);

/*
 * Frees a vector and sets all fields to zero.
 *
 * Defines:
 * - NDEBUG :: Disable assertions
*/
void aoc_vec_free(Vec *ref v);

/*
 * Realloc the size of the vector with custom new capacity.
 *
 * Defines:
 * - NDEBUG :: Disable assertions
 *
 * Errors:
 * - -1 :: max allocation capacity by system is reached
*/
int aoc_vec_reserve(Vec *ref v, size_t new_len);

/*
 * Realloc the size of the vector with default growth factor.
 *
 * Defines:
 * - NDEBUG :: Disable assertions
 *
 * Errors:
 * - -1 :: max allocation capacity by system is reached
*/
int aoc_vec_realloc(Vec *ref v) FN_WARN_UNUSED;

/*
 * Returns values at requested index
 *
 * Defines:
 * - NDEBUG :: Disable assertions
 *
 * Errors:
 * - NULL :: index is bigger than vector length
*/
void *aoc_vec_at(Vec *ref v, size_t index) FN_WARN_UNUSED;

/*
 * Pushes values to vector
 *
 * Defines:
 * - NDEBUG :: Disable assertions
 *
 * Errors:
 * - -1 :: max allocation capacity by system is reached
*/
int aoc_vec_push(Vec *ref v, const void *value) FN_WARN_UNUSED;

/*
 * Pops value from vector to output.
 *
 * Defines:
 * - NDEBUG :: Disable assertions
 *
 * Errors:
 * - -1 :: vector is empty
*/
int aoc_vec_pop(Vec *ref v, void *ref output) FN_WARN_UNUSED;

/*
 * Inserts values to vector at specific index
 *
 * Defines:
 * - NDEBUG :: Disable assertions
 *
 * Errors:
 * - -1 :: index is bigger than vector length
*/
int aoc_vec_insert(Vec *ref v, size_t index, const void *null value) FN_WARN_UNUSED;

/*
 * Swaps the position two values from vector
 *
 * Defines:
 * - NDEBUG :: Disable assertions
*/
void aoc_vec_swap(Vec *ref v, size_t a, size_t b);

/*
 * Partitions the vector
 *
 * Defines:
 * - NDEBUG :: Disable assertions
 *
 * Errors:
 * - -1 :: index is bigger than vector length
*/
size_t aoc_vec_partition(Vec *ref v, VecCompare cmp, size_t lo, size_t hi);

/*
 * Sort part of the vector
 *
 * Defines:
 * - NDEBUG :: Disable assertions
 *
 * Errors:
 * - NULL :: low end is larger than the higher end (lo >= hi)
*/
void aoc_vec_qsort(Vec *ref v, VecCompare cmp, size_t lo, size_t hi);

/*
 * Sort the vector
 *
 * Defines:
 * - NDEBUG :: Disable assertions
 *
 * Errors:
 * - NULL :: low end is larger than the higher end (lo >= hi)
*/
void aoc_vec_sort(Vec *ref v, VecCompare cmp);

/*
 * Shrink the size of the vector, freeing unused memory.
 *
 * Defines:
 * - NDEBUG :: Disable assertions
 *
 * Errors:
 * - -1 :: nothing to shrink (v->len >= v->cap)
*/
int aoc_vec_shrink_to_fit(Vec *ref v) FN_WARN_UNUSED;

/*
 * Clears the vector without freeing memory.
*/
void aoc_vec_clear(Vec *ref v);

/*
 * Pops a value from specified index
 *
 * Defines:
 * - NDEBUG :: Disable assertions
 *
 * Errors:
 * - NULL :: index is bigger than vector length
*/
int aoc_vec_erase(Vec *v, size_t index) FN_WARN_UNUSED;

/*
 * Copies all contents from src to dest
 *
 * Defines:
 * - NDEBUG :: Disable assertions
 *
 * Errors:
 * - -1 :: max allocation capacity by system is reached
 * - -2 :: padding mismatch
*/
int aoc_vec_copy(Vec *xref dest, Vec *xref src) FN_WARN_UNUSED;

/*
 * Checks if vectors are equal in contents, padding and length
 *
 * Defines:
 * - NDEBUG :: Disable assertions
*/
bool aoc_vec_equal(Vec *a, Vec *b, VecCompare cmp);

/*
 * Run function for each element from the vector
*/
void aoc_vec_for_each(Vec *v, void (*fn)(void *));

#endif // AOCLIBS_VEC_H_
