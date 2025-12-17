#ifndef AOCLIBS_COMMON_H
#define AOCLIBS_COMMON_H

#ifndef __clang__
#  define _Nullable
#  define _Nonnull
#endif

// === Strings ===

#define string_literal_len(s) ((sizeof((s)) / sizeof((s)[0])) - sizeof((s)[0]))
#define array_len(a) sizeof(a) / sizeof(a[0])

// === Function Attributes ===

#define COLD_FUNC __attribute__((cold))
#define CONST_FUNC __attribute__((const))
#define DEPRECATED(x) __attribute__((deprecated(x)))
#define HEAP_ALLOCATE __attribute__((malloc))
#define HOT_FUNC __attribute__((hot))
#define NONNULL_RETURN __attribute__((returns_nonnull))
#define PURE_FUNC __attribute__((pure))
#define UNLIKELY(x) __builtin_expect(!!(x), 0)
#define WARN_UNUSED __attribute__((warn_unused_result))
#define NORETURN __attribute__((noreturn))

// === Includes ===

#include <aoclibs/assert.h>
#include <aoclibs/error.h>
#include <aoclibs/int.h>

#endif
