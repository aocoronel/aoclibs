#ifndef AOC_BASE_H_
#define AOC_BASE_H_

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif

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

// Most used LibC libraries
#include "libc.h"
#include "types.h"
#include "cfg.h"
#include "defer.h"

// Assumes all pointers are not nullable. "null" tells the pointer can be NULL.
// NOTE: If an argument accepts NULL or returns NULL, and the annotation is not present, assume it's
// a design bug.
#define null

// Sometimes variables are left undefined on purpose, but there is not indicator of such
#define undefined

// Macros for renaming purposes
#ifdef __cplusplus
#define restrict
#else

#define inline __attribute__((__gnu_inline__)) inline
#define restrict __restrict

#endif // __cplusplus

#ifdef AOC_ENABLE_C11_FEATURES

#ifdef __cplusplus
#define $atomic(type) std::atomic<type>
#else
#define $atomic(type) _Atomic(type)

#ifndef thread_local
#define thread_local _Thread_local
#endif // thread_local

#ifndef static_assert
#define static_assert _Static_assert
#endif // static_assert

#endif // __cplusplus

#else // AOC_ENABLE_C11_FEATURES

#ifndef static_assert
#define static_assert
#endif // static_assert

#endif // AOC_ENABLE_C11_FEATURES

// This macro is an exception on the $macro() convention. Reason: it's a C23 keyword.
#define typeof(type) __typeof__((type))
// C++ auto in C as a GNU extension: #define auto __auto_type
// This will only be enabled when TCC supports it

// All allocations are assumed to be made by malloc() and realloc(), and freed by free()
// Because you can compile the entire library to just a single header file, you can overwrite it
//  #define malloc(size) custom_malloc((size))
// Note: replacing malloc should only be done if you are provided a malloc-like alternative like
// mimalloc, dmalloc, rpmalloc...

// Compiler specific
#define $likely(expr) __builtin_expect((expr), 1)
#define $unlikely(expr) __builtin_expect((expr), 0)
#define $deprecated(fn_to_use_instead) __attribute_deprecated_msg__(fn_to_use_instead)
#define $must_use __attribute__((warn_unused_result))
#define $attr_malloc __attribute__((__malloc__))
#define $attr_printf(x, y) __attribute__((__format__(printf, x, y)))

// The following macros are used to clarify intentions
#define $discard(variable) (void)variable

// $cat(ident, fier) -> identifier
#define $cat(x, y) $$cat(x, y)
#define $$cat(x, y) x##y

// $stringify(identifier) -> "identifier"
#define $stringify(x) #x
// #define identifier 0
// M$stringify(identifier) -> "0"
#define $macro_stringify(x) $stringify(x)

// Using a third variable for swapping, turns this more generic than Xor
#define $swap(x, y)        \
	do {                   \
		typeof(x) tmp = x; \
		x = y;             \
		y = tmp;           \
	} while (0)

// This is only applicable to stack allocated
#define $array_len(array) sizeof((array)) / sizeof((array[0]))

// Compile-time strlen(). This expects a string literal
#define $strlen(string) $array_len(("" string "")) - sizeof((string)[0])

// Unless is used for handling errors, so the control flow is always unlikely to happen
#define $catch(expr) if ($unlikely(expr))

// $range(0, 10) { printf("%d\n", it) }
#define $range(init, end) for (size_t it = (init); it < (end); it++)

#define $abort(msg, ...)                                           \
	(fprintf(stderr, "%s: %s:%u: ", __func__, __FILE__, __LINE__), \
	 fprintf(stderr, msg " " __VA_ARGS__), fputc('\n', stderr), abort())

// Panic when a code is assumed to never run
#define $unreachable(...) $abort("Unreachable code: ", __VA_ARGS__)
#define $panic(...) $abort("Panicked: ", __VA_ARGS__)

// Marks not implemented code
#define $todo(...) $abort("$todo: ", __VA_ARGS__)

// int foo() { $unimplemented; }
#define $unimplemented $abort("Unimplemented function: ", "%s", __func__)

// Asserts an expression, and prints an optional formatted message
#ifdef NDEBUG
#define $assert(...)
#else
#ifdef TUNIT
extern const char *CURRENT_TEST;
static inline void tunit_fail(void);
#define $assert(expr, ...) $tunit_assert(expr, __VA_ARGS__)
#define $tunit_assert(expr, ...)                                                        \
	do {                                                                                \
		if (!(expr)) {                                                                  \
			fprintf(                                                                    \
			    stderr, " %s:%d: Assertion failed in test %s: %s:", __FILE__, __LINE__, \
			    CURRENT_TEST ? CURRENT_TEST : "(unknown)", #expr);                      \
			fprintf(stderr, " " __VA_ARGS__);                                           \
			fputc('\n', stderr);                                                        \
			tunit_fail();                                                               \
			abort();                                                                    \
		}                                                                               \
	} while (0)
#else
#define $assert(expr, ...) ((expr) ? (void)0 : $abort("Assertion failed: " #expr, __VA_ARGS__))
#endif
#endif

// Differently from $assert(), this one is not removed by NDEBUG
#define $expect(expr, ...) ((expr) ? (void)0 : $abort("Assertion failed: " #expr, __VA_ARGS__))

// Convenience assert messages
#define $assert_nonnull(expr) $assert((expr), "passing NULL pointer to Nonnull parameter")

struct Source_Code_Location {
	const char *funcname, *filename;
	int line;
};

#ifdef NDEBUG
#define $$source_code_location
#define $source_code_location
#else
#define $$source_code_location       \
	, (const Source_Code_Location) { \
		__func__, __FILE__, __LINE__ \
	}
#define $source_code_location , const Source_Code_Location source_code_location
#endif

// === Sanitizers ===

// These only take effect with -fsanitize=address

#if defined(__SANITIZE_ADDRESS__)
#include <sanitizer/asan_interface.h>
#define $asan_poison_memory_region(ptr, size) ASAN_POISON_MEMORY_REGION((ptr), (size));
#define $asan_unpoison_memory_region(ptr, size) ASAN_UNPOISON_MEMORY_REGION((ptr), (size));
#else
#define $asan_poison_memory_region(ptr, size)
#define $asan_unpoison_memory_region(ptr, size)
#endif

#if defined(__SANITIZE_MEMORY__)
#include <sanitizer/msan_interface.h>
#define $msan_poison(ptr, size) __msan_poison((ptr), (size))
#define $msan_unpoison(ptr, size) __msan_unpoison((ptr), (size))
#else
#define $msan_poison(ptr, size)
#define $msan_unpoison(ptr, size)
#endif

#if defined(__SANITIZE_MEMORY__) || defined(__SANITIZE_ADDRESS__)
#define HAVE_SANITIZER
#endif

#define $sanitizer_poison_memory(ptr, size)    \
	do {                                       \
		$asan_poison_memory_region(ptr, size); \
		$msan_poison(ptr, size);               \
	} while (0)

#define $sanitizer_unpoison_memory(ptr, size)    \
	do {                                         \
		$asan_unpoison_memory_region(ptr, size); \
		$msan_unpoison(ptr, size);               \
	} while (0)

#endif // AOC_BASE_H_
