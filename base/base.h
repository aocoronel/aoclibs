#ifndef AOCLIBS_BASE_H_
#define AOCLIBS_BASE_H_

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

typedef void *(*aoc_malloc_t)(size_t);
typedef void *(*aoc_realloc_t)(void *, size_t);
typedef void (*aoc_free_t)(void *);

/*
 * Modify prefixes in all functions
*/
#define AOCLIBS_PREFIX

/*
 * The concept of _Nonnull and _Nullable is fascinating and is interesting when combined with
 * assertions, or even with the Clang compiler, thus enforcing if a pointer can or cannot be NULL.
 *
 * This is specially useful, perhaps when a function is never supposed to return NULL or take NULL.
 * A great example is the free function. If we define a custom free wrapper to the free function with
 * a _Nonnull to the pointer and an assertion, the program will immediately stop and warn you have a
 * double free issue.
*/

/*
 * ref (reference) :: aliased to _Nonnull.
 * null :: aliased to _Nullable
*/

// clang-format off

#ifndef __clang__
        #define ref
        #define xref
        #define null
        #define xnull
#else
        #define ref _Nonnull
        #define xref _Nonnull restrict
        #define null _Nullable
        #define xnull _Nullable restrict
#endif

// clang-format on

/*
 * Convenient macros to improve user experience
*/

#define FN_DEPRECATED(fn_to_use_instead) __attribute_deprecated_msg__(fn_to_use_instead)
#define FN_WARN_UNUSED __attribute__((warn_unused_result))

/*
 * Asserts an expression, and prints a formatted message
*/
#ifdef NDEBUG
#define ASSERT(...)
#else
#define ASSERT(exp, ...) \
        ((exp) ? NULL : aoc_assert(#exp, __FILE__, __LINE__, __func__, __VA_ARGS__))
#endif

/*
 * Convenience assert messages
*/
#define ASSERT_NONNULL(exp) ASSERT((exp), "passing NULL pointer to Nonnull parameter")

/*
 * Asserts an expression, and prints a formatted message
*/
AOCLIBS_PREFIX void aoc_assert(const char *expr, const char *file, unsigned line, const char *func,
                               const char *fmt, ...) {
        va_list args;
        va_start(args, fmt);
        fprintf(stderr, "Assertion failed: ");
        vfprintf(stderr, fmt, args);
        fprintf(stderr, "\n%s at %s:%u (%s)\n", expr, file, line, func);
        va_end(args);

        abort();
}

/*
 * Used to panic, when an unreachable code is ran
*/
#define _unreachable aoc_panic(__FILE__, __LINE__, __func__, "unreachable code")

/*
 * Prints message, including metadata and aborts the program
*/
#define panic(msg) aoc_panic(__FILE__, __LINE__, __func__, msg)

/*
 * Prints message, including metadata and aborts the program
 *
 * Should not be used directly. Use the panic macro, instead.
*/
AOCLIBS_PREFIX void aoc_panic(const char *__file, int __line, const char *__func, const char *msg)
        __attribute__((noreturn));
AOCLIBS_PREFIX void aoc_panic(const char *__file, int __line, const char *__func, const char *msg) {
        fprintf(stderr, "PANIC: %s at %s:%d (%s) ", msg, __file, __line, __func);
        fflush(stderr);
        abort();
}

// struct example {
//    size_t len;
//    ...
// };
#define foreach(struct_with_len, it) for (size_t it = 0; it < (struct_with_len)->len; it++)

#define array_len(a) sizeof((a)) / sizeof((a[0]))
#define cstrl_len(s) array_len((" " s " ")) - sizeof((s)[0])

#ifdef AOCLIBS_STRIP_PREFIX
#define swap aoc_swap
#endif

#define aoc_swap(tmp, x, z) \
        do {                \
                tmp = x;    \
                x = z;      \
                z = tmp;    \
        } while (0)

#endif // AOCLIBS_BASE_H_
