#ifndef AOCLIBS_ATTRIBUTES_H
#define AOCLIBS_ATTRIBUTES_H

#ifndef __clang__
#  define _Nonnull
#  define _Nullable
#endif

/*
 * Indicates a variable pointer will change its value
*/
#define mut

/*
 * Indicate a function that can abort the program
*/
#define FN_ABORTS

#define UNLIKELY(x) __builtin_expect(!!(x), 0)

#define FN_COLD __attribute__((cold))
#define FN_CONST __attribute__((const))
#define FN_DEPRECATED(x) __attribute__((deprecated(x)))
#define FN_HEAP_ALLOCATES __attribute__((malloc))
#define FN_HOT __attribute__((hot))
#define FN_NONNULL_RETURN __attribute__((returns_nonnull))
#define FN_NORETURN __attribute__((noreturn))
#define FN_PACKED __attribute__ ((__packed__))
#define FN_PURE __attribute__((pure))
#define FN_WARN_UNUSED __attribute__((warn_unused_result))

#endif
