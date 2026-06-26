#ifndef AOC_RC_H_
#define AOC_RC_H_

#include "base.h"
#include "da.h"

// Maybe rename rc to string? rc may stand for reference counting, but we use as a abbreviation for
// row char, which is nice to type
typedef struct {
	size_t cap;
	size_t len;
	char *data;
} rc;

typedef struct {
	size_t len;
	size_t cap;
	rc *data;
} rcs;

#define rc_appendf(rc, fmt, ...)                                                                 \
	do {                                                                                         \
		int needed = cstr_fmt_size(fmt, __VA_ARGS__);                                            \
		da_reserve((rc), (rc)->len + needed);                                                    \
		int written =                                                                            \
				cstr_fmt_write((rc)->data + (rc)->len, (rc)->cap - (rc)->len, fmt, __VA_ARGS__); \
		(rc)->len += written;                                                                    \
	} while (0)

// Heap concat and appending
#define rc_lcat(rc, items_buff)                                 \
	do {                                                        \
		da_add(rc, items_buff, $strlen(items_buff), (rc)->len); \
		(rc)->len += $strlen(items_buff);                       \
	} while (0)

#define rc_cat(rc, items_buff, items_len)             \
	do {                                              \
		da_add(rc, items_buff, items_len, (rc)->len); \
		(rc)->len += (items_len);                     \
	} while (0)

#define rc_lappend(rc, items_buff) rc_append(rc, items_buff, $strlen((items_buff)))
#define rc_append(rc, items_buff, items_size)                                             \
	do {                                                                                  \
		da_reserve((rc), 1 + (rc)->len + (items_size));                                   \
		(rc)->data[(rc)->len++] = ' ';                                                    \
		memcpy((rc)->data + (rc)->len, (items_buff), (items_size) * sizeof(*(rc)->data)); \
		(rc)->len += (items_size);                                                        \
	} while (0)

#define rc_to_lower(r) cstr_to_lower((r)->data)
#define rcn_to_lower(r) cstrn_to_lower((r)->data, (r)->len)

#define rc_eq(r, pattern) cstr_eq((r)->data, (pattern))
#define rcn_eq(r, pattern, pattern_len) cstrn_eq((r)->data, (r)->len, (pattern), (pattern_len))

#define rc_eq_case(r, pattern) cstr_eq_case((r)->data, (pattern))
#define rcn_eq_case(r, pattern, pattern_len) \
	cstrn_eq_case((r)->data, (r)->len, (pattern), (pattern_len))

#define rc_begins_with(r, begin_len, begin) \
	cstr_begins_with((r).data, (r).len, (begin), (begin_len))
#define rc_ends_with(r, end_len, end) cstr_ends_with((r).data, (r).len, (end), (end_len))

// =================================
// These macros skips the reserve step, for faster operations with less checks.
// Use at own risk.
#define rc_lcat_fast(rc, items_buff, items_len) rc_cat_fast(rc, items_buff, $strlen(items_buff))
#define rc_cat_fast(rc, items_buff, items_len) da_add_fast(rc, items_buff, items_len, (rc)->len)

#define rc_lappend_fast(rc, items_buff, items_size) \
	rc_append_fast(rc, items_buff, $strlen(items_buff))
#define rc_append_fast(rc, items_buff, items_size)                                        \
	do {                                                                                  \
		(rc)->data[(rc)->len++] = ' ';                                                    \
		memcpy((rc)->data + (rc)->len, (items_buff), (items_size) * sizeof(*(rc)->data)); \
		(rc)->len += (items_size);                                                        \
	} while (0)
// =================================

#endif // AOC_RC_H_
