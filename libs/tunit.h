#ifndef AOCLIBS_TUNIT_H_
#define AOCLIBS_TUNIT_H_

// TUnit -- Test Unit

// Enable TUnit :: cc -DTUNIT

/*
 * TEST(my_test_name) :: Register a test
 * TASSERT(1 == 1) :: TUnit specific asserts
 * SKIP_TEST("this must be implemented first") :: Skip registering test
*/

#ifndef TUNIT
#define TEST(...)
#define TASSERT(...)
#define SKIP_TEST(...)
#define TEST_TIMEOUT(...)
#else
#define TEST(desc)                                                   \
    static void test_##desc(void);                                   \
    __attribute__((constructor)) static void register_##desc(void) { \
        tunit_register_test(#desc, test_##desc, 0);                  \
    }                                                                \
    static void test_##desc(void)

#define TEST_TIMEOUT(desc, timeout)                                  \
    static void test_##desc(void);                                   \
    __attribute__((constructor)) static void register_##desc(void) { \
        tunit_register_test(#desc, test_##desc, timeout);            \
    }                                                                \
    static void test_##desc(void)

#define TASSERT(expr, msg)                                      \
    do {                                                        \
        if (!(expr)) {                                          \
            tunit_assert(expr, #expr, msg, __FILE__, __LINE__); \
            return;                                             \
        }                                                       \
    } while (0)

#define SKIP_TEST(reason)        \
    do {                         \
        tunit_skip_test(reason); \
        return;                  \
    } while (0)
#endif

#ifdef HEAP_TRACE
#define no_debug_malloc no_debug_malloc
#define no_debug_free no_debug_free
#define TASSERT_HEAP_TRACE() TASSERT(heap_count_leaks == 0, "memory leak");
#else
#define no_debug_malloc malloc
#define no_debug_free free
#define TASSERT_HEAP_TRACE()
#endif

#ifdef TUNIT
#define _XOPEN_SOURCE 600

#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#ifdef TUNIT_SUBPROCESS
#include <string.h>
#endif

/*
 * Logs formatted message to tunit.txt
*/
static inline void tunit_log(const char *fmt, ...);

typedef void (*TestFunc)(void);

typedef struct __TUnitTest {
    const char *description;
    TestFunc func;
    struct __TUnitTest *next;
    size_t timeout;
} __TUnitTest;

static __TUnitTest *__TUnitHead = NULL;
static __TUnitTest *__TUnitTail = NULL;

static int TESTS_RUN = 0;
static int TESTS_FAIL = 0;
static int TESTS_SKIP = 0;
static const char *CURRENT_TEST = NULL;
static double TESTS_TIME = 0.0;

static jmp_buf __TUnitJMP;
static volatile int __TUnitTimeoutOccurred = 0;

static struct timespec __TUnitStartTime;

FILE *TUNIT_LOG_FILE = NULL;
int tunit_fd = -1;

static inline void __tunit_timeout_handler(int sig) {
    __TUnitTimeoutOccurred = 1;
    fprintf(stderr, "  [TIMEOUT] Test '%s' exceeded time limit (%d)\n", CURRENT_TEST, sig);
    TESTS_FAIL++;
    longjmp(__TUnitJMP, 1);
}

static inline void __tunit_segfault_handler(int sig) {
    fprintf(stderr, "  [CRASH] __tunit_segfault (signal %d) in test '%s'\n", sig, CURRENT_TEST);
    TESTS_FAIL++;
    longjmp(__TUnitJMP, 1);
}
static inline double get_time_diff_ms(struct timespec *start) {
    struct timespec end;
    clock_gettime(CLOCK_MONOTONIC, &end);
    double start_ms = start->tv_sec * 1000.0 + start->tv_nsec / 1000000.0;
    double end_ms = end.tv_sec * 1000.0 + end.tv_nsec / 1000000.0;
    return end_ms - start_ms;
}

#ifdef TUNIT_SUBPROCESS
static inline void __tunit_run_single_test(__TUnitTest *test) {
    CURRENT_TEST = test->description;
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Child process
        close(pipefd[0]); // Close read end

        __TUnitTimeoutOccurred = 0;

        int jump_val = setjmp(__TUnitJMP);
        if (jump_val == 0) {
            signal(SIGSEGV, __tunit_segfault_handler);
            signal(SIGALRM, __tunit_timeout_handler);
            if (test->timeout > 0) {
                alarm(test->timeout);
            }

            test->func();

            alarm(0);
            signal(SIGSEGV, SIG_DFL);

            // Write success to pipe
            write(pipefd[1], "OK", 2);
            close(pipefd[1]);
            exit(EXIT_SUCCESS);
        } else if (jump_val == 1) { /* Assertion fail or crash */
            // Write failure to pipe
            write(pipefd[1], "FAIL", 4);
            close(pipefd[1]);
            exit(EXIT_FAILURE);
        } else if (jump_val == 2) { /* Skipped test */
            // Write skip to pipe
            write(pipefd[1], "SKIP", 4);
            close(pipefd[1]);
            exit(EXIT_SUCCESS);
        }
    } else { // Parent process
        close(pipefd[1]); // Close write end

        char result[5] = { 0 };
        ssize_t n = read(pipefd[0], result, sizeof(result));
        close(pipefd[0]);

        int status;
        waitpid(pid, &status, 0);

        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);

        if (WIFEXITED(status)) {
            clock_gettime(CLOCK_MONOTONIC, &end);
            double duration_ms = get_time_diff_ms(&start);
            if (strcmp(result, "OK") == 0) {
                fprintf(stderr, " ok: %s %.2fms\n", CURRENT_TEST, duration_ms);
            } else if (strcmp(result, "SKIP") == 0) {
                fprintf(stderr, " skip: %s %.2fms\n", CURRENT_TEST, duration_ms);
                TESTS_SKIP++;
            }
            TESTS_TIME += duration_ms;
        } else {
            clock_gettime(CLOCK_MONOTONIC, &end);
            double duration_ms = get_time_diff_ms(&start);
            if (WIFSIGNALED(status)) {
                fprintf(stderr,
                        " fail: %s %.2fms (CRASH: signal %d)\n",
                        CURRENT_TEST,
                        duration_ms,
                        WTERMSIG(status));
            } else {
                fprintf(stderr, " fatal: %s %.2fms (UNKNOWN ERROR)\n", CURRENT_TEST, duration_ms);
            }
            TESTS_TIME += duration_ms;
            TESTS_FAIL++;
        }
    }
}
#else
static inline void __tunit_run_single_test(__TUnitTest *test) {
    double duration_ms = 0.0;
    CURRENT_TEST = test->description;
    __TUnitTimeoutOccurred = 0;

    fprintf(stderr, "  [RUN] %s\r", CURRENT_TEST);
    fflush(stdout);

    clock_gettime(CLOCK_MONOTONIC, &__TUnitStartTime);

    int jump_val = setjmp(__TUnitJMP);
    if (jump_val == 0) {
        signal(SIGSEGV, __tunit_segfault_handler);
        signal(SIGALRM, __tunit_timeout_handler);
        if (test->timeout > 0) {
            alarm(test->timeout);
        }

        test->func();

        alarm(0);
        signal(SIGSEGV, SIG_DFL);

        duration_ms = get_time_diff_ms(&__TUnitStartTime);
        fprintf(stderr, " ok: %s %.2fms\r\n", CURRENT_TEST, duration_ms);
    } else if (jump_val == 1) { /* Assertion fail or crash */
        duration_ms = get_time_diff_ms(&__TUnitStartTime);
        fprintf(stderr, " fail: %s %.2fms\r\n", CURRENT_TEST, duration_ms);
    } else if (jump_val == 2) { /* Skipped test */
        duration_ms = get_time_diff_ms(&__TUnitStartTime);
        fprintf(stderr, " skip: %s %.2fms\r\n", CURRENT_TEST,
                duration_ms); // s for skipped
    }
    TESTS_TIME += duration_ms;
}
#endif
static inline void tunit_register_test(const char *desc, void (*func)(void), size_t timeout) {
    __TUnitTest *tc = no_debug_malloc(sizeof(__TUnitTest));
    tc->description = desc;
    tc->func = func;
    tc->next = NULL;
    tc->timeout = timeout;
    if (__TUnitTail)
        __TUnitTail->next = tc;
    else
        __TUnitHead = tc;
    __TUnitTail = tc;
}

static inline void
tunit_assert(int expr, const char *expr_str, const char *msg, const char *file, int line) {
    if (!expr) {
        fprintf(stderr,
                "  [FAIL] Assertion failed: %s in test %s: %s at %s:%d\n",
                msg,
                CURRENT_TEST ? CURRENT_TEST : "(unknown)",
                expr_str,
                file,
                line);
        TESTS_FAIL++;
        longjmp(__TUnitJMP, 1);
    }
}

static inline void tunit_skip_test(const char *reason) {
    fprintf(stderr, "  [SKIP] Test '%s' skipped: %s\n", CURRENT_TEST, reason);
    TESTS_SKIP++;
    longjmp(__TUnitJMP, 2);
}

static inline void __tunit_run_all_tests(void) {
    __TUnitTest *curr = __TUnitHead;
    while (curr) {
        TESTS_RUN++;
        __tunit_run_single_test(curr);
        curr = curr->next;
    }

    fprintf(stderr,
            "%d succeed, %d failed and %d skipped (%.2fms total)\n",
            TESTS_RUN,
            TESTS_FAIL,
            TESTS_SKIP,
            TESTS_TIME);
}

static inline void __tunit_init_log(const char *log_path) {
    TUNIT_LOG_FILE = fopen(log_path, "a");
    if (!TUNIT_LOG_FILE) {
        perror("failed to open log file");
    }
    tunit_fd = fileno(TUNIT_LOG_FILE);
}

static inline void tunit_log(const char *fmt, ...) {
    if (!TUNIT_LOG_FILE) return;

    va_list args;
    va_start(args, fmt);
    fprintf(TUNIT_LOG_FILE, "%s\n", CURRENT_TEST);
    vfprintf(TUNIT_LOG_FILE, fmt, args);
    va_end(args);
    fflush(TUNIT_LOG_FILE);
}

static inline void __tunit_log(const char *msg) {
    if (!TUNIT_LOG_FILE) return;

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char buffer[100];

    strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", t);

    fprintf(TUNIT_LOG_FILE,
            "===============================================\n=== %s - %s ===\n===============================================\n",
            msg,
            buffer);
    fflush(TUNIT_LOG_FILE);
}

static inline void __tunit_close_log(void) {
    if (TUNIT_LOG_FILE) fclose(TUNIT_LOG_FILE);
    if (tunit_fd != -1) close(tunit_fd);
}

static inline void __tunit_free(void) {
    __TUnitTest *cur = __TUnitHead;
    while (cur) {
        __TUnitTest *next = cur->next;
        no_debug_free(cur);
        cur = next;
    }
    __TUnitHead = NULL;
    __TUnitTail = NULL;
}

int main(void) {
    srand((unsigned)time(NULL));
    char perturb[16];
    snprintf(perturb, sizeof(perturb), "%d", (rand() % 255) + 1);
    setenv("MALLOC_PERTURB_", perturb, 1);

    __tunit_init_log("tunit.log");
    __tunit_log("Initializing test");
    __tunit_run_all_tests();
    __tunit_log("Finished test");
    __tunit_close_log();

    __tunit_free();
    return TESTS_FAIL;
}
#endif // TUNIT

#endif // AOCLIBS_TUNIT_H_
