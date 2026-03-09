#ifndef AOCLIBS_BASE_H_
#define AOCLIBS_BASE_H_

#include <stddef.h>

/*
 * Modify prefixes in all functions:
 * #define AOCDEF static inline
*/
#ifndef AOCDEF
#define AOCDEF
#endif

/*
 * === Aliases ===
*/

/*
 * The concept of _Nonnull and _Nullable is fascinating and is interesting when combined with
 * assertions, or even with the Clang compiler, thus enforcing if a pointer can or cannot be NULL.
 *
 * This is specially useful, perhaps when a function is never supposed to return NULL or take NULL.
*/

/*
 * ref (reference) :: aliased to _Nonnull.
 * null :: aliased to _Nullable
*/

#ifndef __clang__
#define ref
#define xref restrict
#define null
#define xnull restrict
#else
#define ref _Nonnull
#define xref _Nonnull restrict
#define null _Nullable
#define xnull _Nullable restrict
#endif

// To be used in function declarations. "static" is a very broad keyword in C,
// internal express this idea better.
#define internal static

/*
 * === Convenient types ===
*/

typedef void *(*aoc_malloc_t)(size_t);
typedef void *(*aoc_realloc_t)(void *, size_t);
typedef void (*aoc_free_t)(void *);

/*
 * === Attributes ===
*/

#define FN_DEPRECATED(fn_to_use_instead) __attribute_deprecated_msg__(fn_to_use_instead)
#define FN_WARN_UNUSED __attribute__((warn_unused_result))

/*
 * === Debugging ===
*/

#define AOCLIBS_ABORT(msg, ...)                                        \
        (fprintf(stderr, "%s: %s:%u: ", __func__, __FILE__, __LINE__), \
         fprintf(stderr, msg " " __VA_ARGS__),                         \
         fputc('\n', stderr),                                          \
         abort())

// Used to panic, when an unreachable code is ran
#define UNREACHABLE AOCLIBS_ABORT("Panicked: ", "unreachable code")

// Prints message, including metadata and aborts the program
#define PANIC(...) AOCLIBS_ABORT("Panicked: ", __VA_ARGS__)

// Marks not implemented code
#define TODO(...) AOCLIBS_ABORT("TODO: ", __VA_ARGS__)

// Asserts an expression, and prints a formatted message
#ifdef NDEBUG
#define ASSERT(...)
#else
#define ASSERT(expr, ...) \
        ((expr) ? (void)0 : AOCLIBS_ABORT("Assertion failed: " #expr, __VA_ARGS__))
#endif

// Convenience assert messages
#define ASSERT_NONNULL(exp) ASSERT((exp), "passing NULL pointer to Nonnull parameter")

/*
 * Convenient macros
*/

#define swap(Type, x, z)    \
        do {                \
                Type t = x; \
                x = z;      \
                z = t;      \
        } while (0)

// This is only applicable to stack allocated
#define ARRAY_LEN(a) sizeof((a)) / sizeof((a[0]))
// Expects a string literal
#define STRLEN(s) ARRAY_LEN(("" s "")) - sizeof((s)[0])

#define eprintf(...) fprintf(stderr, __VA_ARGS__)

/*
 * === "Keywords" ===
*/

// struct example {
//    size_t len;
//    ...
// };
//
// struct example myexample = {0};
// foreach (&myexample, i) {
//      printf("%d\n", i);
// }
#define foreach(struct_with_len, it) for (size_t it = 0; it < (struct_with_len)->len; it++)

#endif // AOCLIBS_BASE_H_
