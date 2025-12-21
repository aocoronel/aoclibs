#ifndef AOCLIBS_TUNIT_H
#define AOCLIBS_TUNIT_H

// === Includes ===

#ifdef TUNIT
#define _XOPEN_SOURCE 600
#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

// === Declarations ===

#ifdef TUNIT
extern FILE *TUNIT_LOG_FILE;

void tunit_register_test(const char *desc, void (*func)(void));
void tunit_assert(int expr, const char *expr_str, const char *msg,
                  const char *file, int line);
void tunit_skip_test(const char *reason);
void tunit_run_all_tests(void);
void tunit_log(const char *fmt, ...);
#endif

// === Macros ===

#ifndef TUNIT
#define TEST(...)
#define TASSERT(...)
#define SKIP_TEST(...)
#else
#define TEST(desc)                                                       \
        static void test_##desc(void);                                   \
        __attribute__((constructor)) static void register_##desc(void) { \
                tunit_register_test(#desc, test_##desc);                 \
        }                                                                \
        static void test_##desc(void)

#define TASSERT(expr, msg)                                                  \
        do {                                                                \
                if (!(expr)) {                                              \
                        tunit_assert(expr, #expr, msg, __FILE__, __LINE__); \
                        return;                                             \
                }                                                           \
        } while (0)

#define SKIP_TEST(reason)                                        \
        do {                                                     \
                tunit_skip_test(reason);                         \
                return;                                          \
        } while (0)
#endif

#endif /* AOCLIBS_TUNIT_H */
