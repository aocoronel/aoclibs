#ifndef AOCLIBS_STRING_H_
#define AOCLIBS_STRING_H_

#include "attributes.h"
#include <alloca.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define string_literal_len(s) ((sizeof((s)) / sizeof((s)[0])) - sizeof((s)[0]))
#define array_len(a) sizeof(a) / sizeof(a[0])

#define StrError ((size_t)-1)

typedef enum {
        StringHeap,
        StringStack,
        StringLiteral,
} StringType;

#define get_slice(s) (s.len), (s.slice)
typedef struct {
        const char *slice;
        int len;
} StrSlice;

typedef struct {
        char *str;
        size_t len;
        size_t cap;
        StringType type;
} str;

#define str_to_slice(s, x, y) cstr_to_slice((s.str), (x), (y))
StrSlice cstr_to_slice(const char *ref s, size_t start, size_t end);

bool str_can_mut(const str *null s);

bool str_is_null(const str *null s);

bool str_is_null_assert(const str *null s);

#define cstr_dup(s, len) _cstr_dup((s), (len + 1))
char *xnull _cstr_dup(const char *ref s, const size_t len);

str str_dup(const str *ref s);

void cstr_to_lower(char *ref s, const size_t len);

#define str_eq(s1, s2) cstr_eq((s1.str), (s2.str))
#define str_eq_case(s1, s2) cstr_eq_case((s1.str), (s2.str))

bool cstr_eq(const char *xref s1, const char *xref s2);

bool cstr_eq_case(const char *xref s1, const char *xref s2);

#define str_len_comptime(s) ((sizeof((" " s " ")) / sizeof((s)[0])) - sizeof((s)[0]))

size_t str_len(const char *null s, const size_t buff);

#define str_new_stack(cap) _str_new_stack(alloca((cap)), cap)
str _str_new_stack(char *ref s, const size_t cap);

#define str_new_comptime(s) _str_new_comptime((" " s " "), str_len_comptime(s))
str _str_new_comptime(char *ref s, const size_t len);

#define str_new_heap(cap) _str_new_heap(malloc((cap)), (cap))
str _str_new_heap(char *null s, const size_t cap);

int str_resize(str *ref s, size_t cap);

void str_free(str *ref s);

void str_free_array(str *ref s[], size_t len);

void str_erase(str *ref s);

void str_clear(str *ref s);

#define cstr_begins_with(s, begin, begin_len, s_len) \
        cstr_match_pos((s), (begin), (begin_len), (s_len), 0)
#define cstr_ends_with(s, end, end_len, s_len) \
        cstr_match_pos((s), (end), (end_len), (s_len), (s_len) - (end_len))
bool cstr_match_pos(const char *xnull s, const char *xnull pattern, size_t pattern_len,
                    size_t s_len, size_t offset);

#define str_overwrite(s1, s2) str_copy((s1), (s2), 0)
#define str_cat(s1, s2) str_copy((s1), (s2), s1.len)
#define str_append(s1, s2) str_copy((s1), (s2), (s1.len + 1))
int str_copy(str *xref s1, const str *xref s2, const size_t s1_offset);

int str_push(str *ref s, char c);

int str_pop(str *ref s);

int str_drop(str *ref s, size_t index);

#define cster_overwrite(s1, s2, s2_len) cstr_copy((s1), (s2), 0, s2_len)
#define cster_overwrite_comptime(s1, s2) cstr_copy((s1), (s2), 0, str_len_comptime(s2))
#define cstr_cat(s1, s2) cstr_copy((s1), (s2), s1.len, str_len((s2)))
#define cstr_cat_comptime(s1, s2) cstr_copy((s1), (s2), s1.len, str_len_comptime((s2)))
#define cstr_append(s1, s2) cstr_copy((s1), (s2), (s1.len + 1), str_len((s2)))
#define cstr_append_comptime(s1, s2) cstr_copy((s1), (s2), (s1.len + 1), str_len_comptime((s2)))
#define str_null_terminate(s) cstr_copy((s), "\0", (*s.len), 1)
int cstr_copy(str *xref s1, const char *xref s2, const size_t s1_offset, size_t s2_len);

int cstr_copy_fmt(str *xref s, const char *xref fmt, ...);

bool match_delim_rec(const char *xnull s, size_t s_len, const char *xnull delim, size_t delim_len,
                     size_t pos, size_t j);

size_t str_chr_str(const str *xref s, const char *xref delim, const size_t delim_len);

size_t str_chr(const str *ref s, char delim);

const char *null str_tok_str_const(const str *xref s, const char *xref delim,
                                   const size_t delim_len);

const char *null str_tok(const str *ref s, char delim);

size_t cstr_trim_whitespace(char *ref s, const size_t len);

void str_trim_whitespace(str *ref s);

double cstr_to_double(const char *ref s, const double _default);

bool cstr_to_bool(const char *ref s, const bool _default);

float cstr_to_float(const char *ref s, const float _default);

long cstr_to_long(const char *ref s, const long _default);

#endif // AOCLIBS_STRING_H_
