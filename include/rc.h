#ifndef AOCLIBS_RC_H_
#define AOCLIBS_RC_H_

#include "base.h"
#include "cstr.h"
#include "da.h"
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
// rc myrc = rcs_bnew(buff, STRLEN("Hello, world"));
#define aoc_rcs_bnew(buff, length)                                   \
        (rc) {                                                       \
                .data = (buff), .len = length, .cap = sizeof((buff)) \
        }

// Allocates a new stack/heap RC.
// rc myrc = rc_new(100);
#define aoc_rc_new(capacity)                                                        \
        (rc) {                                                                      \
                .data = AOCLIBS_DA_REALLOC((capacity)), .len = 0, .cap = (capacity) \
        }

// rc myrc = rcs_new(100);
#define aoc_rcs_new(capacity)                                           \
        (rc) {                                                          \
                .data = alloca((capacity)), .len = 0, .cap = (capacity) \
        }

#define aoc_rc_fmt_append(rc, fmt, ...)                                                   \
        do {                                                                              \
                int needed = aoc_cstrcpy_fmt_size(fmt, __VA_ARGS__);                      \
                aoc_da_reserve((rc), (rc)->len + needed);                                 \
                int written = aoc_cstrcpy_fmt(                                            \
                        (rc)->data + (rc)->len, (rc)->cap - (rc)->len, fmt, __VA_ARGS__); \
                (rc)->len += written;                                                     \
        } while (0)

// Stack concat and appending
#define aoc_rcls_cat(rc, items_buff) aoc_das_copy(rc, items_buff, STRLEN(items_buff))
#define aoc_rcs_cat(rc, items_buff, items_size) aoc_das_copy(rc, items_buff, items_size)

#define aoc_rcls_append(rc, items_buff) aoc_rcs_append(rc, items_buff, STRLEN(items_buff))
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
#define aoc_rcl_cat(rc, items_buff) aoc_da_add(rc, items_buff, STRLEN(items_buff), (rc)->len)
#define aoc_rc_cat(rc, items_buff, items_len) aoc_da_add(rc, items_buff, items_len, (rc)->len)

#define aoc_rcl_append(rc, items_buff) aoc_rc_append(rc, items_buff, STRLEN((items_buff)))
#define aoc_rc_append(rc, items_buff, items_size)                                                 \
        do {                                                                                      \
                aoc_da_reserve((rc), 1 + (rc)->len + (items_size));                               \
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

// =================================
// These macros skips the reserve step, for faster operations with less checks.
// Use at own risk.
#define aoc_rc_cat_fast(rc, items_buff, items_len) \
        aoc_da_add_fast(rc, items_buff, items_len, (rc)->len)

#define aoc_rc_append_fast(rc, items_buff, items_size)                                            \
        do {                                                                                      \
                (rc)->data[(rc)->len++] = ' ';                                                    \
                memcpy((rc)->data + (rc)->len, (items_buff), (items_size) * sizeof(*(rc)->data)); \
                (rc)->len += (items_size);                                                        \
        } while (0)
// =================================

#ifdef AOCLIBS_STRIP_PREFIX
#define rc_bnew aoc_rc_bnew
#define rc_new aoc_rc_new

#define rcs_bnew aoc_rcs_bnew
#define rcs_new aoc_rcs_new

#define rcs_cat aoc_rcs_cat
#define rcls_cat aoc_rcls_cat

#define rcs_append aoc_rcs_append
#define rcls_append aoc_rcls_append

#define rc_append aoc_rcs_append
#define rcl_append aoc_rcls_append

#define rc_cat aoc_rcs_cat
#define rcl_cat aoc_rcls_cat

#define rc_cat_fast aoc_rc_cat_fast
#define rc_append_fast aoc_rc_append_fast

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
