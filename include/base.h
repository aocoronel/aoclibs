#ifndef AOCLIBS_BASE_H_
#define AOCLIBS_BASE_H_

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif

// Most used LibC libraries
#include "libc.h"

// AOCDEF can modify prefixes in all functions. Example:
//  #define AOCDEF static inline
//  #define AOCDEF extern "C"
//
// AOCLIBS won't use the "static" keyword to make functions "private", instead we just take the
// convention of starting these functions with "_". The idea is that the provided functions can be
// further reused.
#ifndef AOCDEF
#define AOCDEF
#endif

// Assumes all pointers are not nullable. "null" tells the pointer can be NULL.
// NOTE: If an argument accepts NULL or returns NULL, and the annotation is not present, assume it's
// a design bug.
#define null

// All allocations are assumed to be made by malloc() and realloc(), and freed by free()
// Because you can compile the entire library to just a single header file, you can overwrite it
//  #define malloc(size) custom_malloc((size))

// Compiler specific
#define LIKELY(expr) __builtin_expect(!!(expr), 1)
#define UNLIKELY(expr) __builtin_expect(!!(expr), 0)
#define NORETURN __attribute__((noreturn))
#define DEPRECATED(fn_to_use_instead) __attribute_deprecated_msg__(fn_to_use_instead)
#define MUST_USE __attribute__((warn_unused_result))

// The following macros are used to clarify intentions
#define DISCARD(variable) (void)variable

// Macros for renaming purposes
#define restrict __restrict
#define typeof(type) __typeof__((type))
// C++ auto in C as a GNU extension: #define auto __auto_type

// Convenient macros
#define eprintf(...) fprintf(stderr, __VA_ARGS__)

// CAT(ident, fier) -> identifier
#define CAT(x, y) CAT_IMPL(x, y)
#define CAT_IMPL(x, y) x##y

// STRINGIFY(identifier) -> "identifier"
#define STRINGIFY(x) #x
// #define identifier 0
// MSTRINGIFY(identifier) -> "0"
#define MACRO_STRINGIFY(x) STRINGIFY(x)

// Using a third variable for swapping, turns this more generic than Xor
#define SWAP(x, y)         \
	do {                   \
		typeof(x) tmp = x; \
		x = y;             \
		y = tmp;           \
	} while (0)

// This is only applicable to stack allocated
#define ARRAY_LEN(array) sizeof((array)) / sizeof((array[0]))

// Compile-time strlen(). This expects a string literal
#define STRLEN(string) ARRAY_LEN(("" string "")) - sizeof((string)[0])

// Unless is used for handling errors, so the control flow is always unlikely to happen
#define unless(expr) if (UNLIKELY(expr))

// range(0, 10, i) { printf("%d\n", i) }
#define range(init, end, it) for (size_t it = (init); it < (end); it++)

// aoclibs' abort()
#define AOCLIBS_ABORT(msg, ...)                                    \
	(fprintf(stderr, "%s: %s:%u: ", __func__, __FILE__, __LINE__), \
	 fprintf(stderr, msg " " __VA_ARGS__),                         \
	 fputc('\n', stderr),                                          \
	 abort())

// Panic when a code is assumed to never run
#define UNREACHABLE(...) AOCLIBS_ABORT("Unreachable code: ", __VA_ARGS__)
#define PANIC(...) AOCLIBS_ABORT("Panicked: ", __VA_ARGS__)

// Marks not implemented code
#define TODO(...) AOCLIBS_ABORT("TODO: ", __VA_ARGS__)

// int foo() { UNIMPLEMENTED; }
#define UNIMPLEMENTED AOCLIBS_ABORT("Unimplemented function: ", __func__)

// Asserts an expression, and prints an optional formatted message
#ifdef NDEBUG
#define ASSERT(...)
#else
#define ASSERT(expr, ...) \
	((expr) ? (void)0 : AOCLIBS_ABORT("Assertion failed: " #expr, __VA_ARGS__))
#endif

// Differently from ASSERT(), this one is not removed by NDEBUG
#define EXPECT(expr, ...) \
	((expr) ? (void)0 : AOCLIBS_ABORT("Assertion failed: " #expr, __VA_ARGS__))

// Convenience assert messages
#define ASSERT_NONNULL(expr) ASSERT((expr), "passing NULL pointer to Nonnull parameter")

// === Sanitizers ===

// These only take effect with -fsanitize=address

#if defined(__SANITIZE_ADDRESS__)
#include <sanitizer/asan_interface.h>
#define _ASAN_POISON_MEMORY_REGION(ptr, size) ASAN_POISON_MEMORY_REGION((ptr), (size));
#define _ASAN_UNPOISON_MEMORY_REGION(ptr, size) ASAN_UNPOISON_MEMORY_REGION((ptr), (size));
#else
#define _ASAN_POISON_MEMORY_REGION(ptr, size)
#define _ASAN_UNPOISON_MEMORY_REGION(ptr, size)
#endif

#if defined(__SANITIZE_MEMORY__)
#include <sanitizer/msan_interface.h>
#define __MSAN_POISON(ptr, size) __msan_poison((ptr), (size))
#define __MSAN_UNPOISON(ptr, size) __msan_unpoison((ptr), (size))
#else
#define __MSAN_POISON(ptr, size)
#define __MSAN_UNPOISON(ptr, size)
#endif

#if defined(__SANITIZE_MEMORY__) || defined(__SANITIZE_ADDRESS__)
#define HAVE_SANITIZER
#endif

#define sanitizer_poison_memory(ptr, size)     \
	do {                                       \
		_ASAN_POISON_MEMORY_REGION(ptr, size); \
		__MSAN_POISON(ptr, size);              \
	} while (0)

#define sanitizer_unpoison_memory(ptr, size)     \
	do {                                         \
		_ASAN_UNPOISON_MEMORY_REGION(ptr, size); \
		__MSAN_UNPOISON(ptr, size);              \
	} while (0)

#endif // AOCLIBS_BASE_H_
