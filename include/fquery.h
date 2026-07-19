#ifndef AOC_FQUERY_H_
#define AOC_FQUERY_H_

#include "base.h"

// FQuery takes the same use case of regular expressions, however, without compilation.
//
// Setting up FQuery is simple:
//
// int skip_to_a = 'a';
// const FQuery *query =
//      $fquery(
//       { .fn = fquery_any, .arg = NULL },
//       { .fn = fquery_any, .arg = NULL },
//       { .fn = fquery_skip_to_int, .arg = &skip_to_a });
// fquery(query, "00adasd")

#define $fquery(...)      \
	(FQuery[]) {          \
		__VA_ARGS__, NULL \
	}

typedef const char *(*FQuery_Fn)(const char *null, const void *);

struct FQuery {
	FQuery_Fn fn;
	const void *arg;
};

// *p == c
AOCDEF const char *null fquery_eq_int(const char *null p, const void *c);

// regex: "*.c"
AOCDEF const char *null fquery_skip_to_int(const char *null p, const void *c);
AOCDEF const char *null fquery_skip_to_fn(const char *null p, const void *c);

// regex: "?"
AOCDEF const char *null fquery_optional(const char *null p, const void *c);

// regex: "."
AOCDEF const char *null fquery_any(const char *null p, const void *c);

// Wrappers to ctype.h
AOCDEF const char *null fquery_isalnum(const char *null p, const void *c);
AOCDEF const char *null fquery_isalpha(const char *null p, const void *c);
AOCDEF const char *null fquery_iscntrl(const char *null p, const void *c);
AOCDEF const char *null fquery_isdigit(const char *null p, const void *c);
AOCDEF const char *null fquery_isgraph(const char *null p, const void *c);
AOCDEF const char *null fquery_islower(const char *null p, const void *c);
AOCDEF const char *null fquery_isprint(const char *null p, const void *c);
AOCDEF const char *null fquery_ispunct(const char *null p, const void *c);
AOCDEF const char *null fquery_isspace(const char *null p, const void *c);
AOCDEF const char *null fquery_isupper(const char *null p, const void *c);
AOCDEF const char *null fquery_isxdigit(const char *null p, const void *c);

AOCDEF bool fquery_recursive(const FQuery *qms, const char *null p);

AOCDEF bool fquery(const FQuery *qms, const char *pattern);

#ifdef AOC_IMPLEMENTATION
#include "fquery.c"
#endif // AOC_IMPLEMENTATION

#endif
