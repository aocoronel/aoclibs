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
// lrc --> row char literal
// rcs --> row char stack
// rch --> row char heap
//
// The rcn macros means the same as strnlen, strncpy and strncat

// Compile-time known RC, which cannot be mutated
// rc myrc = lrc_new("hello, world!");
#define aoc_lrc_new(s)                                                          \
        (rc) {                                                                  \
                .data = " "(s) " ", .len = lcstrlen((s)), .cap = lcstrlen((s)), \
        }

// RC from already allocated char * in the stack/heap
// char *buff = malloc(100);
// rc myrc = rch_bnew(buff, 0, 100);
#define aoc_rch_bnew(buff, length, capacity)                       \
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
// rc myrc = rch_new(100);
#define aoc_rch_new(capacity)                                              \
        (rc) {                                                             \
                .data = RC_MALLOC((capacity)), .len = 0, .cap = (capacity) \
        }

// rc myrc = _rch_new(malloc, 100);
#define _aoc_rch_new(allocator, capacity)                                  \
        (rc) {                                                             \
                .data = allocator((capacity)), .len = 0, .cap = (capacity) \
        }

// rc myrc = rcs_new(100);
#define aoc_rcs_new(capacity)                                           \
        (rc) {                                                          \
                .data = alloca((capacity)), .len = 0, .cap = (capacity) \
        }

#define aoc_rcs_append rcs_append
#define aoc_lrcs_append(rc, items_buff) _aoc_rcs_append(rc, (items_buff), lcstrlen(items_buff))

#define _aoc_rcs_append(rc, items_buff, items_size)             \
        ((rc)->len + items_size > (rc)->cap) {                  \
                _aoc_assert_da_is_not_null(rc);                 \
                memcpy((rc)->data + (rc)->len, (items_buff),    \
                       (items_size) * sizeof(*(rc)->data) - 1); \
                (rc)->len += (items_size);                      \
        }

#define aoc_rc_to_lower(r) aoc_cstr_to_lower((r)->data)
#define aoc_rcn_to_lower(r) aoc_cstr_to_lower((r)->data, (r)->len)

#define aoc_rc_eq(r, pattern) aoc_cstr_eq((r)->data, (pattern))
#define aoc_rcn_eq(r, pattern, pattern_len) \
        aoc_cstrn_eq((r)->data, (r)->len, (pattern), (pattern_len))

#define aoc_rc_eq_case(r, pattern) aoc_cstr_eq_case((r)->data, (pattern))
#define aoc_rcn_eq_case(r, pattern, pattern_len) \
        aoc_cstrn_eq_case((r)->data, (r)->len, (pattern), (pattern_len))

#define rc_begins_with(r, begin_len, begin) \
        aoc_cstr_match_pos((r).len, (r).data, (begin_len), (begin), 0)
#define rc_ends_with(r, end_len, end) \
        aoc_cstr_match_pos((r).len, (r).data, (end_len), (end), (r).len - (end_len))

#define aoc_rc_begins_with(r, begin, begin_len) \
        aoc_cstrn_eq((r)->data, (r)->len, (begin), (begin_len))
#define aoc_rc_ends_with(r, end, end_len) \
        aoc_cstrn_eq((r)->data + (r)->len, (r)->len, (end), (end_len))

#ifdef AOCLIBS_STRIP_PREFIX
#define lrc_new aoc_lrc_new

#define _rch_new _aoc_rch_new
#define rch_bnew aoc_rch_bnew
#define rch_new aoc_rch_new

#define rcs_bnew aoc_rcs_bnew
#define rcs_new aoc_rcs_new

#define rcs_append aoc_rcs_append
#define lrcs_append laoc_rcs_append

#define rc_to_lower aoc_rc_to_lower
#define rcn_to_lower aoc_rcn_to_lower

#define rc_eq aoc_rc_eq
#define rcn_eq aoc_rcn_eq
#define rc_eq_case aoc_rc_eq_case
#define rcn_eq_case aoc_rcn_eq_case
#endif

#endif // AOCLIBS_RC_H_
