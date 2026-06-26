#pragma once

#include "cstr.h"
#include "base.h"
#include <ctype.h>

char *cstr_dup(const char *s, const size_t len) {
	$assert_nonnull(s != NULL);

	char *d = (char *)malloc(len);
	$catch(!d) return NULL;

	memcpy(d, s, len);
	return d;
}

void cstr_to_lower(char *s) {
	$assert_nonnull(s != NULL);
	for (; *s; s++)
		*s = tolower(*s);
}

void cstrn_to_lower(char *s, const size_t len) {
	$assert_nonnull(s != NULL);
	$range(0, len, i) s[i] = tolower(s[i]);
}

void cstr_to_upper(char *s) {
	$assert_nonnull(s != NULL);
	for (; *s; s++)
		*s = toupper(*s);
}

void cstrn_to_upper(char *s, const size_t len) {
	$assert_nonnull(s != NULL);
	$range(0, len, i) s[i] = toupper(s[i]);
}

bool cstr_ends_with(const char *s,
					const size_t s_len,
					const char *pattern,
					const size_t pattern_len) {
	$assert_nonnull(s);
	$assert_nonnull(pattern);
	$catch(s_len < pattern_len) return false;
	return memcmp(s + s_len - pattern_len, pattern, pattern_len) == 0;
}

bool cstr_begins_with(const char *s,
					  const size_t s_len,
					  const char *pattern,
					  const size_t pattern_len) {
	$assert_nonnull(s);
	$assert_nonnull(pattern);
	return cstrn_eq(s, s_len, pattern, pattern_len);
}

bool cstr_eq(const char *s, const char *pattern) {
	$assert_nonnull(s);
	$assert_nonnull(pattern);
	size_t s_len = strlen(s);
	size_t pattern_len = strlen(pattern);
	$catch(pattern_len == 0 || pattern_len > s_len) return false;
	$catch(s_len < pattern_len) return false;
	return memcmp(s, pattern, pattern_len) == 0;
}

bool cstrn_eq(const char *s, const size_t s_len, const char *pattern, const size_t pattern_len) {
	$assert_nonnull(s);
	$assert_nonnull(pattern);
	$catch(pattern_len == 0 || pattern_len > s_len) return false;
	$catch(s_len < pattern_len) return false;
	return memcmp(s, pattern, pattern_len) == 0;
}

char cstr_eq_case(const char *s, const char *pattern) {
	$assert_nonnull(s);
	$assert_nonnull(pattern);
	size_t s_len = strlen(s);
	size_t pattern_len = strlen(pattern);
	$catch(pattern_len == 0 || pattern_len > s_len) return false;

	char *ptr = (char *)malloc(sizeof(char) * s_len + pattern_len);
	$catch(!ptr) return -1;

	char *s_tmp = ptr;
	char *pattern_tmp = ptr + s_len;

	memcpy(s_tmp, s, s_len);
	memcpy(pattern_tmp, pattern, pattern_len);

	cstrn_to_lower(s_tmp, s_len);
	cstrn_to_lower(pattern_tmp, pattern_len);

	bool equal = memcmp(s_tmp, pattern_tmp, pattern_len) == 0;

	free(ptr);

	return equal;
}

char cstrn_eq_case(const char *s,
				   const size_t s_len,
				   const char *pattern,
				   const size_t pattern_len) {
	$assert_nonnull(s);
	$assert_nonnull(pattern);
	$catch(pattern_len == 0 || pattern_len > s_len) return false;

	char *ptr = (char *)malloc(sizeof(char) * s_len + pattern_len);
	$catch(!ptr) return -1;

	char *s_tmp = ptr;
	char *pattern_tmp = ptr + s_len;

	memcpy(s_tmp, s, s_len);
	memcpy(pattern_tmp, pattern, pattern_len);

	cstrn_to_lower(s_tmp, s_len);
	cstrn_to_lower(pattern_tmp, pattern_len);

	bool equal = memcmp(s_tmp, pattern_tmp, pattern_len) == 0;

	free(ptr);

	return equal;
}

bool cstr_has(const char *s, const size_t s_len, const char *pattern, const size_t pattern_len) {
	$assert_nonnull(s);
	$assert_nonnull(pattern);
	$catch(pattern_len == 0 || pattern_len > s_len) return false;

	const char *s_ptr = s;
	size_t remaining_len = s_len;

	$catch(pattern_len == 1) return memchr(s_ptr, *pattern, s_len) != NULL;

	while ((s_ptr = (const char *)memchr(s_ptr, pattern[0], remaining_len)) != NULL) {
		remaining_len = s_len - (s_ptr - s);
		if (remaining_len >= pattern_len && memcmp(s_ptr, pattern, pattern_len) == 0) return true;
		s_ptr++;
		remaining_len--;
	}

	return false;
}

size_t
cstr_has_at(const char *s, const size_t s_len, const char *pattern, const size_t pattern_len) {
	$assert_nonnull(s);
	$assert_nonnull(pattern);
	$catch(pattern_len == 0 || pattern_len > s_len) return false;

	const char *s_ptr = s;
	size_t remaining_len = s_len;

	$catch(pattern_len == 1) {
		s_ptr = (const char *)memchr(s_ptr, *pattern, s_len);
		remaining_len = s_len - (s_ptr - s);
		return s_len - remaining_len;
	}

	while ((s_ptr = (const char *)memchr(s_ptr, pattern[0], remaining_len)) != NULL) {
		remaining_len = s_len - (s_ptr - s);
		if (remaining_len >= pattern_len && memcmp(s_ptr, pattern, pattern_len) == 0)
			return s_len - remaining_len;
		s_ptr++;
		remaining_len--;
	}

	return SIZE_MAX;
}

size_t index_of(const char *s, char delim, size_t size) {
	$assert_nonnull(s != NULL);

	const char *ptr = (const char *)memchr(s, delim, size);
	$catch(ptr == NULL) return SIZE_MAX;

	return ptr - s;
}

int cstr_fmt_size(const char *fmt, ...) {
	$assert_nonnull(fmt != NULL);

	va_list args;
	va_start(args, fmt);
	int needed_len = vsnprintf(NULL, 0, fmt, args);
	va_end(args);

	return needed_len;
}

int cstr_fmt_write(char *s, const size_t s_cap, const char *fmt, ...) {
	$assert_nonnull(s != NULL);
	$assert_nonnull(fmt != NULL);

	int allocated_len = 0;

	va_list args;
	va_start(args, fmt);
	allocated_len = vsnprintf(s, s_cap, fmt, args);
	va_end(args);

	return allocated_len;
}

double cstr_to_double(const char *s, const double _default) {
	$assert_nonnull(s != NULL);
	char *endptr;
	double val = strtod(s, &endptr);
	$catch(*endptr != '\0') return _default;
	return val;
}

bool cstr_to_bool(const char *s, const bool _default) {
	$assert_nonnull(s != NULL);
	if (cstr_eq_case(s, "true") || cstr_eq(s, "1")) return true;
	if (cstr_eq_case(s, "false") || cstr_eq(s, "0")) return false;
	return _default;
}

float cstr_to_float(const char *s, const float _default) {
	$assert_nonnull(s != NULL);
	char *endptr;
	float val = strtof(s, &endptr);
	$catch(*endptr != '\0') return _default;
	return val;
}

long cstr_to_long(const char *s, const long _default) {
	$assert_nonnull(s != NULL);
	char *endptr;
	long val = strtol(s, &endptr, 10);
	$catch(*endptr != '\0') return _default;
	return val;
}

Slice cstr_to_slice_from(const char *str, const size_t start, const size_t end) {
	return (Slice){ .data = str + start, .len = end - start };
}

Slice cstr_to_slice(const char *str) {
	size_t len = strlen(str);
	return (Slice){ .data = str, .len = len };
}

bool slice_eq(Slice lhs, Slice rhs) {
	$assert_nonnull(lhs.data);
	$assert_nonnull(rhs.data);
	$catch(rhs.len == 0 || rhs.len > lhs.len) return false;
	$catch(lhs.len < rhs.len) return false;
	return memcmp(lhs.data, rhs.data, lhs.len) == 0;
}

char slice_eq_case(Slice lhs, Slice rhs) {
	$assert_nonnull(lhs.data);
	$assert_nonnull(rhs.data);
	$catch(rhs.len == 0 || rhs.len > lhs.len) return false;

	char *ptr = (char *)malloc(sizeof(char) * lhs.len + rhs.len);
	$catch(!ptr) return -1;

	char *s_tmp = ptr;
	char *pattern_tmp = ptr + lhs.len;

	memcpy(s_tmp, lhs.data, lhs.len);
	memcpy(pattern_tmp, rhs.data, rhs.len);

	cstrn_to_lower(s_tmp, lhs.len);
	cstrn_to_lower(pattern_tmp, rhs.len);

	bool equal = memcmp(s_tmp, pattern_tmp, rhs.len) == 0;

	free(ptr);

	return equal;
}

bool slice_begins_with(Slice lhs, Slice rhs) {
	return slice_eq(lhs, rhs);
}

bool slice_begins_with_cstr(Slice lhs, const char *rhs, const size_t rhs_len) {
	return cstrn_eq(lhs.data, lhs.len, rhs, rhs_len);
}

bool slice_ends_with(Slice lhs, Slice rhs) {
	$assert_nonnull(lhs.data);
	$assert_nonnull(rhs.data);
	$catch(lhs.len < rhs.len) return false;
	return memcmp(lhs.data + lhs.len - rhs.len, rhs.data, rhs.len) == 0;
}

bool slice_ends_with_cstr(Slice lhs, const char *rhs, const size_t rhs_len) {
	$assert_nonnull(lhs.data);
	$assert_nonnull(rhs);
	$catch(lhs.len < rhs_len) return false;
	return memcmp(lhs.data + lhs.len - rhs_len, rhs, rhs_len) == 0;
}

Slice while_next_word(const char *restrict s, size_t *restrict begin, size_t end) {
	$assert_nonnull(s);
	size_t i = *begin;

	while (i < end && isspace((unsigned char)s[i])) {
		i++;
	}

	size_t start = i;

	while (i < end && !isspace((unsigned char)s[i])) {
		i++;
	}

	*begin = i;

	return (Slice){ .data = s + start, .len = i - start };
}

Slice while_next_word_and(const char *restrict s, size_t *restrict begin, size_t end, char delim) {
	$assert_nonnull(s);

	size_t i = *begin;

	while (i < end && isspace((unsigned char)s[i])) {
		i++;
	}

	size_t start = i;

	bool found_delim = false;

	while (i < end && !isspace((unsigned char)s[i])) {
		if (s[i] == delim) {
			found_delim = true;
			break;
		}
		i++;
	}

	*begin = i;

	if (found_delim) {
		return (Slice){ .data = s + start, .len = i - start };
	} else {
		return (Slice){ .data = s + start, .len = i - start };
	}
}

void slice_to_cstr(Slice s, char *buff, const size_t size) {
	int size_to_copy = s.len > size ? size : s.len;
	memcpy(buff, s.data, size_to_copy);
	buff[size_to_copy] = '\0';
}

size_t cstr_skip_whitespace_forward(const char *pos, size_t len) {
	$assert_nonnull(pos);
	size_t i = 0;
	while (i < len && isspace((unsigned char)pos[i]))
		i++;
	return i;
}

size_t cstr_skip_whitespace_backward(const char *pos, size_t len) {
	$assert_nonnull(pos);
	size_t i = 0;
	while (i < len && isspace((unsigned char)pos[len - 1 - i]))
		i++;
	return i;
}

void slice_skip_whitespace_backward(Slice *s) {
	size_t skipped_whitesapce = cstr_skip_whitespace_backward(s->data, s->len);
	s->len -= skipped_whitesapce;
}

void slice_skip_whitespace_forward(Slice *s) {
	size_t skipped_whitesapce = cstr_skip_whitespace_forward(s->data, s->len);
	s->data += skipped_whitesapce;
	s->len -= skipped_whitesapce;
}

void slice_trim(Slice *s) {
	slice_skip_whitespace_forward(s);
	slice_skip_whitespace_backward(s);
}

void slice_chop_right_by(Slice *s, char delim) {
	$assert_nonnull(s);
	const char *ptr = (const char *)memchr(s->data, delim, s->len);
	$catch(!ptr) return;

	s->len = (size_t)(ptr - s->data);
}

Slice slice_extract_from_substring(Slice *s) {
	$assert_nonnull(s);
	$catch(s->len == 0 || s->data[0] != '"') {
		return *s;
	}

	const char *pos = s->data + 1;
	size_t len = 0;

	while (len < s->len - 1) {
		if (pos[len] == '\\') {
			if (len + 1 < s->len - 1)
				len += 2;
			else
				break;
		} else if (pos[len] == '"') {
			break;
		} else {
			len++;
		}
	}

	return (Slice){
		.data = pos,
		.len = len,
	};
}

void slice_shift_by(Slice *s, size_t len) {
	s->data += len;
	s->len -= len;
}
