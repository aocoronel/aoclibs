#pragma once

#include "base.h"
#include "fquery.h"
#include <ctype.h>

AOCDEF const char *null fquery_eq_int(const char *null p, const void *c) {
	$assert_nonnull(c);

	int _c = *(int *)c;
	if (!p || *p != _c) {
		return NULL;
	}
	return p + 1;
}

AOCDEF const char *null fquery_skip_to_fn(const char *null p, const void *c) {
	$assert_nonnull(c);

	int (*fn)(int) = (int (*)(int))c;
	const char *tmp = p;
	while (tmp && fn(*tmp) == 0) {
		tmp++;
	}
	tmp++;
	return tmp;
}

AOCDEF const char *null fquery_skip_to_int(const char *null p, const void *c) {
	$assert_nonnull(c);

	int _c = *(int *)c;
	return (const char *)memchr(p, _c, 1);
}

AOCDEF const char *null fquery_optional(const char *null p, const void *c) {
	$assert_nonnull(c);
	if (!p) return NULL;

	int _c = *(int *)c;
	if (*p != _c) {
		return p;
	}
	return p + 1;
}

// regex: "."
AOCDEF const char *null fquery_any(const char *null p, const void *c) {
	$discard(c);

	if (!p || *p == '\0') {
		return NULL;
	}
	return p + 1;
}

// regex: "[0-9]"
AOCDEF const char *null fquery_isdigit(const char *null p, const void *c) {
	$discard(c);

	if (!p || !isdigit((unsigned char)*p)) {
		return NULL;
	}
	return p + 1;
}

AOCDEF const char *null fquery_isalnum(const char *null p, const void *c) {
	$discard(c);

	if (!p || !isalnum((unsigned char)*p)) {
		return NULL;
	}
	return p + 1;
}

AOCDEF const char *null fquery_isspace(const char *null p, const void *c) {
	$discard(c);

	if (!p || !isspace((unsigned char)*p)) {
		return NULL;
	}
	return p + 1;
}

AOCDEF const char *null fquery_iscntrl(const char *null p, const void *c) {
	$discard(c);

	if (!p || !iscntrl((unsigned char)*p)) {
		return NULL;
	}
	return p + 1;
}

AOCDEF const char *null fquery_islower(const char *null p, const void *c) {
	$discard(c);

	if (!p || !islower((unsigned char)*p)) {
		return NULL;
	}
	return p + 1;
}

AOCDEF const char *null fquery_isgraph(const char *null p, const void *c) {
	$discard(c);

	if (!p || !isgraph((unsigned char)*p)) {
		return NULL;
	}
	return p + 1;
}

AOCDEF const char *null fquery_isprint(const char *null p, const void *c) {
	$discard(c);

	if (!p || !isprint((unsigned char)*p)) {
		return NULL;
	}
	return p + 1;
}

AOCDEF const char *null fquery_ispunct(const char *null p, const void *c) {
	$discard(c);

	if (!p || !ispunct((unsigned char)*p)) {
		return NULL;
	}
	return p + 1;
}

AOCDEF const char *null fquery_isxdigit(const char *null p, const void *c) {
	$discard(c);

	if (!p || !isxdigit((unsigned char)*p)) {
		return NULL;
	}
	return p + 1;
}

AOCDEF const char *null fquery_isupper(const char *null p, const void *c) {
	$discard(c);

	if (!p || !isupper((unsigned char)*p)) {
		return NULL;
	}
	return p + 1;
}

AOCDEF const char *null fquery_isalpha(const char *null p, const void *c) {
	$discard(c);

	if (!p || !isalpha((unsigned char)*p)) {
		return NULL;
	}
	return p + 1;
}

AOCDEF bool fquery_recursive(const FQuery *qms, const char *null p) {
	if (qms->fn == NULL) return true;

	const FQuery_Fn fn = qms->fn;
	const void *arg = qms->arg;

	const char *next = fn(p, arg);

	if (!next) return false;

	return fquery_recursive(qms + 1, next);
}

AOCDEF bool fquery(const FQuery *qms, const char *pattern) {
	$assert_nonnull(qms);
	$assert_nonnull(pattern);
	return fquery_recursive(qms, pattern);
}

// int main(void) {
//     int skip_to_a = 'a';
//     int optional_dash = 'b';
//     const FQuery *qms = new_fquery({ .fn = fquery_skip_to_fn, .arg = isdigit },
//                                      { .fn = fquery_optional, .arg = &optional_dash },
//                                      { .fn = fquery_skip_to_int, .arg = &skip_to_a });
//
//     printf("Query: 'any, any, skip_to_a'\n");
//     printf("  Matching '1ba': Found: %s\n", fquery(qms, "1ba") ? "true" : "false");
//     printf("  Matching 'bba': Found: %s\n", fquery(qms, "bba") ? "true" : "false");
//     printf("  Matching 'bb': Found: %s\n", fquery(qms, "bb") ? "true" : "false");
//     printf("  Matching 'a': Found: %s\n", fquery(qms, "a") ? "true" : "false");
//     printf("  Matching 'bbbc': Found: %s\n", fquery(qms, "bbbc") ? "true" : "false");
//
//     const FQuery *qms2 = new_fquery(
//             { .fn = fquery_isdigit }, { .fn = fquery_isdigit }, { .fn = fquery_isdigit });
//
//     printf("\nQuery: 'isdigit, isdigit, isdigit'\n");
//     printf("  Matching '123': Found: %s\n", fquery(qms2, "123") ? "true" : "false");
//     printf("  Matching '12a': Found: %s\n", fquery(qms2, "12a") ? "true" : "false");
//
//     return 0;
// }
