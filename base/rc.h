#ifndef AOCLIBS_RC_H_
#define AOCLIBS_RC_H_

#include "base.h"
#include "cstr.h"
#include <alloca.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Row Char
// This is Dynamic Array compatible. This implementation is suppose to give you a 'String' type
// which provides safer operations. It can work with string literals, stack allocated and heap
// allocated strings. If you decide to use this like an Dynamic Array, make sure to just use it
// with a heap RC.
typedef struct {
        size_t cap;
        size_t len;
        char *data;
} rc;

#ifndef RC_MALLOC
#define RC_MALLOC malloc
#endif

// In short:
// rcs --> row char stack
// rc --> row char heap
//
// The rcn macros means the same as strnlen, strncpy and strncat

// RC from already allocated char * in the stack/heap
// char *buff = malloc(100);
// rc myrc = rc_bnew(buff, 0, 100);
#define aoc_rc_bnew(buff, length, capacity)                        \
        (rc) {                                                     \
                .data = (buff), .len = (length), .cap = (capacity) \
        }

// char buff[100];
// rc myrc = rcs_bnew(buff, 0);
//
// char buff[100] = "Hello, world!";
// rc myrc = rcs_bnew(buff, lcstrlen("Hello, world"));
#define aoc_rcs_bnew(buff, length)                                   \
        (rc) {                                                       \
                .data = (buff), .len = length, .cap = sizeof((buff)) \
        }

// Allocates a new stack/heap RC.
// rc myrc = rc_new(100);
#define aoc_rc_new(capacity)                                               \
        (rc) {                                                             \
                .data = RC_MALLOC((capacity)), .len = 0, .cap = (capacity) \
        }

// rc myrc = _rc_new(malloc, 100);
#define _aoc_rc_new(allocator, capacity)                                   \
        (rc) {                                                             \
                .data = allocator((capacity)), .len = 0, .cap = (capacity) \
        }

// rc myrc = rcs_new(100);
#define aoc_rcs_new(capacity)                                           \
        (rc) {                                                          \
                .data = alloca((capacity)), .len = 0, .cap = (capacity) \
        }

// Stack concat and appending
#define aoc_lrcs_cat(rc, items_buff) aoc_das_copy(rc, items_buff, lcstrlen(items_buff))
#define aoc_rcs_cat(rc, items_buff, items_size) aoc_das_copy(rc, items_buff, items_size)

#define aoc_lrcs_append(rc, items_buff) aoc_rcs_append(rc, items_buff, lcstrlen(items_buff))
#define aoc_rcs_append(rc, items_buff, items_size)            \
        ((rc)->len + items_size + 1 < (rc)->cap) {            \
                _aoc_assert_da_is_not_null(rc);               \
                _aoc_assert_da_data_is_not_null(rc);          \
                (rc)->data[(rc)->len++] = ' ';                \
                memcpy((rc)->data + (rc)->len,                \
                       (items_buff),                          \
                       (items_size) * (sizeof(*(rc)->data))); \
                (rc)->len += (items_size);                    \
        }

// Heap concat and appending
#define aoc_lrc_cat(rc, items_buff) \
        aoc_da_add(AOCLIBS_DA_REALLOC, rc, items_buff, lcstrlen(items_buff), (rc)->len)
#define _aoc_lrc_cat(realloc, rc, items_buff) \
        aoc_da_add(realloc, rc, items_buff, lcstrlen(items_buff), (rc)->len)

#define aoc_rc_cat(rc, items_buff, items_len) \
        aoc_da_add(AOCLIBS_DA_REALLOC, rc, items_buff, items_len, (rc)->len)
#define _aoc_rc_cat(realloc, rc, items_buff, items_len) \
        aoc_da_add(realloc, rc, items_buff, items_len, (rc)->len)

#define aoc_lrc_append(rc, items_buff) \
        _aoc_rcs_append(AOCLIBS_DA_REALLOC, rc, items_buff, lcstrlen((items_buff)))
#define _aoc_lrc_append(realloc, rc, items_buff) \
        _aoc_rcs_append(realloc, rc, items_buff, lcstrlen((items_buff)))

#define aoc_rc_append(rc, items_buff, items_len) \
        _aoc_rc_append(AOCLIBS_DA_REALLOC, rc, items_buff, items_len)
#define _aoc_rc_append(realloc, rc, items_buff, items_size)                                       \
        do {                                                                                      \
                _aoc_da_reserve(realloc, (rc), 1 + (rc)->len + (items_size));                     \
                (rc)->data[(rc)->len++] = ' ';                                                    \
                memcpy((rc)->data + (rc)->len, (items_buff), (items_size) * sizeof(*(rc)->data)); \
                (rc)->len += (items_size);                                                        \
        } while (0)

#define aoc_rc_to_lower(r) aoc_cstr_to_lower((r)->data)
#define aoc_rcn_to_lower(r) aoc_cstrn_to_lower((r)->data, (r)->len)

#define aoc_rc_eq(r, pattern) aoc_cstr_eq((r)->data, (pattern))
#define aoc_rcn_eq(r, pattern, pattern_len) \
        aoc_cstrn_eq((r)->data, (r)->len, (pattern), (pattern_len))

#define aoc_rc_eq_case(r, pattern) aoc_cstr_eq_case((r)->data, (pattern))
#define aoc_rcn_eq_case(r, pattern, pattern_len) \
        aoc_cstrn_eq_case((r)->data, (r)->len, (pattern), (pattern_len))

#define aoc_rc_begins_with(r, begin_len, begin) \
        aoc_cstr_begins_with((r).data, (r).len, (begin), (begin_len))
#define aoc_rc_ends_with(r, end_len, end) aoc_cstr_ends_with((r).data, (r).len, (end), (end_len))

#ifdef AOCLIBS_STRIP_PREFIX
#define _rc_new _aoc_rc_new
#define rc_bnew aoc_rc_bnew
#define rc_new aoc_rc_new

#define rcs_bnew aoc_rcs_bnew
#define rcs_new aoc_rcs_new

#define rcs_cat aoc_rcs_cat
#define lrcs_cat aoc_lrcs_cat

#define rcs_append aoc_rcs_append
#define lrcs_append aoc_lrcs_append

#define rc_append aoc_rcs_append
#define lrc_append aoc_lrcs_append

#define rc_cat aoc_rcs_cat
#define lrc_cat aoc_lrcs_cat

#define rc_to_lower aoc_rc_to_lower
#define rcn_to_lower aoc_rcn_to_lower

#define rc_eq aoc_rc_eq
#define rcn_eq aoc_rcn_eq
#define rc_eq_case aoc_rc_eq_case
#define rcn_eq_case aoc_rcn_eq_case

#define rc_begins_with aoc_rc_begins_with
#define rc_ends_with aoc_rc_ends_with
#endif

#endif // AOCLIBS_RC_H_
