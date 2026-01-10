#ifndef AOCLIBS_ASSERT_H_
#define AOCLIBS_ASSERT_H_

#include "attributes.h"

/*
 * Asserts an expression, and prints a formatted message
*/
#ifdef NDEBUG
#define ASSERT(...)
#else
#define ASSERT(exp, ...) \
        (void)((!!(exp)) || (aoc_assert(#exp, __FILE__, __LINE__, __func__, __VA_ARGS__), 0))
#endif

/*
 * Convenience assert messages
*/
#define ASSERT_NONNULL(exp) ASSERT((exp), "passing NULL pointer to Nonnull parameter")

/*
 * Asserts an expression, and prints a formatted message
*/
AOCLIBS_PREFIX void aoc_assert(const char *expr, const char *file, unsigned line, const char *func,
                               const char *fmt, ...);
#endif // AOCLIBS_ASSERT_H_
