#ifndef AOCLIBS_BASE_H_
#define AOCLIBS_BASE_H_

#define _XOPEN_SOURCE 700

// This is the foundation for all libraries

#include "libc.h"

// Modify prefixes in all functions:
// #define AOCDEF static inline
#ifndef AOCDEF
#define AOCDEF
#endif

// === Pointers ===

// Assumes all pointers at not nullable. Most, but not all, will be asserted.

// "null" tells the pointer can be NULL
#define null

#define restrict __restrict

// === Allocations ===

// All allocations are assumed to be made by malloc(), realloc() and freed by free()
// Because you can compile the entire library to just a single header file, you can use
// the "heap_trace.h" trick to change it.

// === Attributes ===

#define LIKELY(x) __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)

#define ATTR_ALLOCS(x) __attribute__((malloc, alloc_size(x)))

#define ATTR_COLD __attribute__((cold))
#define ATTR_HOT __attribute__((hot))

#define ATTR_NORETURN __attribute__((noreturn))

#define ATTR_PACKED __attribute__((__packed__))

#define ATTR_CONST __attribute__((const))
#define ATTR_PURE __attribute__((pure))

#define ATTR_DEPRECATED(fn_to_use_instead) __attribute_deprecated_msg__(fn_to_use_instead)
#define ATTR_WARN_UNUSED __attribute__((warn_unused_result))

// To be used in function declarations. "static" is a very broad keyword in C,
// internal express this idea better.
#define internal __attribute__((visibility("hidden")));

#define DISCARD(x) (void)x

// === Debugging ===

#define AOCLIBS_ABORT(msg, ...)                                    \
    (fprintf(stderr, "%s: %s:%u: ", __func__, __FILE__, __LINE__), \
     fprintf(stderr, msg " " __VA_ARGS__),                         \
     fputc('\n', stderr),                                          \
     abort())

// Used to panic, when an unreachable code is ran
#define UNREACHABLE AOCLIBS_ABORT("Panicked: ", "unreachable code")

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

// Convenient macros

#define CAT(a, b) CAT_IMPL(a, b)
#define CAT_IMPL(a, b) a##b

#define STRINGIFY(x) #x

// #define hello 0
// MSTRINGIFY(hello) -> "0"
#define MSTRINGIFY(x) STRINGIFY(x)

#define swap(x, z)           \
    do {                     \
        __typeof__(x) t = x; \
        x = z;               \
        z = t;               \
    } while (0)

// This is only applicable to stack allocated
#define ARRAY_LEN(a) sizeof((a)) / sizeof((a[0]))
// Expects a string literal
#define STRLEN(s) ARRAY_LEN(("" s "")) - sizeof((s)[0])

#define eprintf(...) fprintf(stderr, __VA_ARGS__)

// void foo() {
//     void *p = malloc(1 << 20);
//     catch (!p) abort();
// }
#define catch(expr) if (UNLIKELY(expr))

#define range(init, end, it) for (size_t it = (init); it < (end); it++)

// Sanitizers

#if defined(__SANITIZE_ADDRESS__)
#include <sanitizer/asan_interface.h>
#define _ASAN_POISON_MEMORY_REGION(region, size) ASAN_POISON_MEMORY_REGION((region), (size));
#define _ASAN_UNPOISON_MEMORY_REGION(region, size) ASAN_UNPOISON_MEMORY_REGION((region), (size));
#else
#define _ASAN_POISON_MEMORY_REGION(region, size)
#define _ASAN_UNPOISON_MEMORY_REGION(region, size)
#endif

#if defined(__SANITIZE_MEMORY__)
#include <sanitizer/msan_interface.h>
#define __MSAN_POISON(region, size) __msan_poison((region), (size))
#define __MSAN_UNPOISON(region, size) __msan_unpoison((region), (size))
#else
#define __MSAN_POISON(region, size)
#define __MSAN_UNPOISON(region, size)
#endif

#if defined(__SANITIZE_MEMORY__) || defined(__SANITIZE_ADDRESS__)
#define HAVE_SANITIZER
#endif

#define sanitizer_poison_memory(region, size)     \
    do {                                          \
        _ASAN_POISON_MEMORY_REGION(region, size); \
        __MSAN_POISON(region, size);              \
    } while (0)

#define sanitizer_unpoison_memory(region, size)     \
    do {                                            \
        _ASAN_UNPOISON_MEMORY_REGION(region, size); \
        __MSAN_UNPOISON(region, size);              \
    } while (0)

#endif // AOCLIBS_BASE_H_
