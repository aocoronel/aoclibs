#ifndef AOC_TUNIT_H_
#define AOC_TUNIT_H_

// TUnit -- Test Unit
//
// This library is standalone from aoclibs. Taking this single file will work anywhere.

// Enable TUnit :: cc -DTUNIT

/*
 * TEST(my_test_name) :: Register a test
 * $assert(1 == 1) :: TUnit specific asserts
 * SKIP_TEST("this must be implemented first") :: Skip registering test
*/

#ifndef TUNIT
#define TEST(...)
#define $assert(...)
#define SKIP_TEST(...)
#define TEST_TIMEOUT(...)
#else
#define TEST(desc)                                                         \
    static void test_##desc(void);                                         \
    __attribute__((constructor)) static void tunit_register_##desc(void) { \
        tunit_register_test(#desc, test_##desc, 0);                        \
    }                                                                      \
    static void test_##desc(void)

#define TEST_TIMEOUT(desc, timeout)                                        \
    static void test_##desc(void);                                         \
    __attribute__((constructor)) static void tunit_register_##desc(void) { \
        tunit_register_test(#desc, test_##desc, timeout);                  \
    }                                                                      \
    static void test_##desc(void)

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

#define SKIP_TEST(desc)                                                    \
    static void test_##desc(void);                                         \
    __attribute__((constructor)) static void tunit_register_##desc(void) { \
        return;                                                            \
    }                                                                      \
    static void test_##desc(void)
#endif

#define $assert_heap_trace() $assert(heap_count_leaks == 0, "memory leak");

#ifdef TUNIT
#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700

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

void heap_trace_summary(FILE *fd);
int heap_count_leaks(void);

// Logs formatted message to tunit.txt
static inline void tunit_log(const char *fmt, ...);

typedef void (*TestFunc)(void);

typedef struct TUnit_Test {
    const char *description;
    TestFunc func;
    struct TUnit_Test *next;
    size_t timeout;
} TUnit_Test;

static TUnit_Test *TUNIT_HEAD = NULL;
static TUnit_Test *TUNIT_TAIL = NULL;

static int TESTS_RUN = 0;
static int TESTS_FAIL = 0;
static int TESTS_SKIP = 0;
static int TESTS_LEAKS = 0;
static double TESTS_TIME = 0.0;
static const char *CURRENT_TEST = NULL;

static jmp_buf TUNIT_JUMP;
static volatile int TUNIT_TIMEOUT_OCCURRED = 0;

static struct timespec TUNIT_START_TIME;

FILE *TUNIT_LOG_FILE = NULL;
int TUNIT_FD = -1;

static inline void tunit_timeout_handler(int sig) {
    TUNIT_TIMEOUT_OCCURRED = 1;
    fprintf(stderr, "  [timeout] Test '%s' exceeded time limit (%d)\n", CURRENT_TEST, sig);
    TESTS_FAIL++;
    longjmp(TUNIT_JUMP, 1);
}

static inline void tunit_segfault_handler(int sig) {
    fprintf(stderr, "  [segmentation fault] (signal %d) in test '%s'\n", sig, CURRENT_TEST);
    TESTS_FAIL++;
    longjmp(TUNIT_JUMP, 1);
}
static inline double tunit_get_time_diff_ms(struct timespec *start) {
    struct timespec end;
    clock_gettime(CLOCK_MONOTONIC, &end);
    double start_ms = start->tv_sec * 1000.0 + start->tv_nsec / 1000000.0;
    double end_ms = end.tv_sec * 1000.0 + end.tv_nsec / 1000000.0;
    return end_ms - start_ms;
}

#ifdef TUNIT_SUBPROCESS
static inline void tunit_run_single_test(TUnit_Test *test) {
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

        TUNIT_TIMEOUT_OCCURRED = 0;

        int jump_val = setjmp(TUNIT_JUMP);
        if (jump_val == 0) {
            signal(SIGSEGV, tunit_segfault_handler);
            signal(SIGALRM, tunit_timeout_handler);
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
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);

        close(pipefd[1]); // Close write end

        char result[5] = { 0 };
        ssize_t n = read(pipefd[0], result, sizeof(result));
        close(pipefd[0]);

        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            clock_gettime(CLOCK_MONOTONIC, &end);
            double duration_ms = tunit_get_time_diff_ms(&start);
            if (strcmp(result, "OK") == 0) {
                fprintf(stderr, " ok: %s %.2fms\r\n", CURRENT_TEST, duration_ms);
            } else if (strcmp(result, "SKIP") == 0) {
                fprintf(stderr, " skip: %s\r\n", CURRENT_TEST);
                TESTS_SKIP++;
            }
            TESTS_TIME += duration_ms;
        } else {
            clock_gettime(CLOCK_MONOTONIC, &end);
            double duration_ms = tunit_get_time_diff_ms(&start);
            if (WIFSIGNALED(status)) {
                fprintf(
                    stderr, " fail: %s %.2fms (CRASH: signal %d)\r\n", CURRENT_TEST, duration_ms,
                    WTERMSIG(status));
            } else {
                fprintf(stderr, " fatal: %s %.2fms (Unknown error)\r\n", CURRENT_TEST, duration_ms);
            }
            TESTS_TIME += duration_ms;
            TESTS_FAIL++;
        }
    }
}
#else
static inline void tunit_run_single_test(TUnit_Test *test) {
    double duration_ms = 0.0;
    int leaks_begin = heap_count_leaks();
    CURRENT_TEST = test->description;
    TUNIT_TIMEOUT_OCCURRED = 0;

    clock_gettime(CLOCK_MONOTONIC, &TUNIT_START_TIME);

    int jump_val = setjmp(TUNIT_JUMP);
    if (jump_val == 0) {
        signal(SIGSEGV, tunit_segfault_handler);
        signal(SIGALRM, tunit_timeout_handler);
        if (test->timeout > 0) {
            alarm(test->timeout);
        }

        test->func();

        alarm(0);
        signal(SIGSEGV, SIG_DFL);

        duration_ms = tunit_get_time_diff_ms(&TUNIT_START_TIME);

        int leaks = heap_count_leaks() - leaks_begin;
        if (leaks > 0) {
            TESTS_LEAKS++;
            fprintf(stderr, " ok: %s %.2fms (%d leaks)\r\n", CURRENT_TEST, duration_ms, leaks);
        } else {
            fprintf(stderr, " ok: %s %.2fms\r\n", CURRENT_TEST, duration_ms);
        }
    } else if (jump_val == 1) { /* Assertion fail or crash */
        duration_ms = tunit_get_time_diff_ms(&TUNIT_START_TIME);

        int leaks = heap_count_leaks() - leaks_begin;
        if (leaks > 0) {
            TESTS_LEAKS++;
            fprintf(stderr, " fail: %s %.2fms (%d leaks)\r\n", CURRENT_TEST, duration_ms, leaks);
        } else {
            fprintf(stderr, " fail: %s %.2fms\r\n", CURRENT_TEST, duration_ms);
        }
    } else if (jump_val == 2) { /* Skipped test */
        duration_ms = tunit_get_time_diff_ms(&TUNIT_START_TIME);

        int leaks = heap_count_leaks() - leaks_begin;
        if (leaks > 0) {
            TESTS_LEAKS++;
            fprintf(stderr, " skip: %s %.2fms (%d leaks)\r\n", CURRENT_TEST, duration_ms, leaks);
        } else {
            fprintf(stderr, " skip: %s %.2fms\r\n", CURRENT_TEST, duration_ms);
        }
    }
    TESTS_TIME += duration_ms;
}
#endif
static inline void tunit_register_test(const char *desc, void (*func)(void), size_t timeout) {
    TUnit_Test *tc = malloc(sizeof(TUnit_Test));
    tc->description = desc;
    tc->func = func;
    tc->next = NULL;
    tc->timeout = timeout;
    if (TUNIT_TAIL)
        TUNIT_TAIL->next = tc;
    else
        TUNIT_HEAD = tc;
    TUNIT_TAIL = tc;
}

static inline void tunit_fail(void) {
    TESTS_FAIL++;
    longjmp(TUNIT_JUMP, 1);
}

static inline void tunit_run_all_tests(void) {
    TUnit_Test *curr = TUNIT_HEAD;
    while (curr) {
        TESTS_RUN++;
        tunit_run_single_test(curr);
        curr = curr->next;
    }

    fprintf(
        stderr, "%d succeed, %d failed (%.2fms total)\n", TESTS_RUN - TESTS_FAIL, TESTS_FAIL,
        TESTS_TIME);

    heap_trace_summary(stderr);
}

static inline void tunit_init_log(const char *log_path) {
    TUNIT_LOG_FILE = fopen(log_path, "a");
    if (!TUNIT_LOG_FILE) {
        perror("failed to open log file");
    }
    TUNIT_FD = fileno(TUNIT_LOG_FILE);
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

static inline void tunit_log_begin_end(const char *msg) {
    if (!TUNIT_LOG_FILE) return;

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char buffer[100];

    strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", t);

    fprintf(
        TUNIT_LOG_FILE,
        "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n    %s - %s\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n",
        msg, buffer);
    fflush(TUNIT_LOG_FILE);
}

static inline void tunit_close_log(void) {
    if (TUNIT_LOG_FILE) fclose(TUNIT_LOG_FILE);
    if (TUNIT_FD != -1) close(TUNIT_FD);
}

static inline void tunit_free(void) {
    TUnit_Test *cur = TUNIT_HEAD;
    while (cur) {
        TUnit_Test *next = cur->next;
        free(cur);
        cur = next;
    }
    TUNIT_HEAD = NULL;
    TUNIT_TAIL = NULL;
}

int main(void) {
    tunit_init_log("tunit.log");
    tunit_log_begin_end("Initializing test");
    tunit_run_all_tests();
    tunit_log_begin_end("Finished test");
    tunit_close_log();

    tunit_free();
    return TESTS_FAIL;
}
#endif // TUNIT

#endif // AOC_TUNIT_H_
