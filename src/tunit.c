#include <aoclibs/tunit.h>

#ifdef TUNIT

#define _XOPEN_SOURCE 600
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

typedef void (*TestFunc)(void);

typedef struct TestCase {
        const char *description;
        TestFunc func;
        struct TestCase *next;
} TestCase;

static TestCase *TESTS_HEAD = NULL;
static TestCase *TESTS_TAIL = NULL;
static int TESTS_RUN = 0;
static int TESTS_FAIL = 0;
static int TESTS_SKIP = 0;
static const char *CURRENT_TEST = NULL;

static jmp_buf JMP_BUFFER;
static volatile int TIMEOUT_OCCURRED = 0;

static struct timespec TEST_START_TIME;

FILE *TUNIT_LOG_FILE = NULL;
int tunit_fd = -1;

void timeout_handler(int sig) {
        TIMEOUT_OCCURRED = 1;
        fprintf(stderr, "  [TIMEOUT] Test '%s' exceeded time limit (%d)\n",
                CURRENT_TEST, sig);
        TESTS_FAIL++;
        longjmp(JMP_BUFFER, 1);
}

void segfault_handler(int sig) {
        fprintf(stderr, "  [CRASH] SEGFAULT (signal %d) in test '%s'\n", sig,
                CURRENT_TEST);
        TESTS_FAIL++;
        longjmp(JMP_BUFFER, 1);
}

static double get_time_diff_ms(struct timespec *start) {
        struct timespec end;
        clock_gettime(CLOCK_MONOTONIC, &end);
        double start_ms = start->tv_sec * 1000.0 + start->tv_nsec / 1000000.0;
        double end_ms = end.tv_sec * 1000.0 + end.tv_nsec / 1000000.0;
        return end_ms - start_ms;
}

#ifdef TUNIT_SUBPROCESS
static void run_single_test(TestCase *test) {
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

                TIMEOUT_OCCURRED = 0;

                int jump_val = setjmp(JMP_BUFFER);
                if (jump_val == 0) {
                        signal(SIGSEGV, segfault_handler);
                        signal(SIGALRM, timeout_handler);
                        alarm(5); // 5 second timeout

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
                        if (strcmp(result, "OK") == 0) {
                                clock_gettime(CLOCK_MONOTONIC, &end);
                                double duration_ms = get_time_diff_ms(&start);
                                fprintf(stderr, " ✓ %s %.2fms\n", CURRENT_TEST,
                                        duration_ms);
                        } else if (strcmp(result, "SKIP") == 0) {
                                clock_gettime(CLOCK_MONOTONIC, &end);
                                double duration_ms = get_time_diff_ms(&start);
                                fprintf(stderr, " s %s %.2fms\n", CURRENT_TEST,
                                        duration_ms);
                                TESTS_SKIP++;
                        }
                } else {
                        clock_gettime(CLOCK_MONOTONIC, &end);
                        double duration_ms = get_time_diff_ms(&start);
                        if (WIFSIGNALED(status)) {
                                fprintf(stderr,
                                        " ✗ %s %.2fms (CRASH: signal %d)\n",
                                        CURRENT_TEST, duration_ms,
                                        WTERMSIG(status));
                        } else {
                                fprintf(stderr,
                                        " ✗ %s %.2fms (UNKNOWN ERROR)\n",
                                        CURRENT_TEST, duration_ms);
                        }
                        TESTS_FAIL++;
                }
        }
}
#else
static void run_single_test(TestCase *test) {
        CURRENT_TEST = test->description;
        TIMEOUT_OCCURRED = 0;

        fprintf(stderr, "  [RUN] %s\r", CURRENT_TEST);
        fflush(stdout);

        clock_gettime(CLOCK_MONOTONIC, &TEST_START_TIME);

        int jump_val = setjmp(JMP_BUFFER);
        if (jump_val == 0) {
                signal(SIGSEGV, segfault_handler);
                signal(SIGALRM, timeout_handler);
                alarm(30); // 30 second timeout

                test->func();

                alarm(0);
                signal(SIGSEGV, SIG_DFL);

                double duration_ms = get_time_diff_ms(&TEST_START_TIME);
                fprintf(stderr, " ✓ %s %.2fms\r\n", CURRENT_TEST, duration_ms);
        } else if (jump_val == 1) { /* Assertion fail or crash */
                double duration_ms = get_time_diff_ms(&TEST_START_TIME);
                fprintf(stderr, " ✗ %s %.2fms\r\n", CURRENT_TEST, duration_ms);
        } else if (jump_val == 2) { /* Skipped test */
                double duration_ms = get_time_diff_ms(&TEST_START_TIME);
                fprintf(stderr, " s %s %.2fms\r\n", CURRENT_TEST,
                        duration_ms); // s for skipped
        }
}
#endif

void tunit_register_test(const char *desc, void (*func)(void)) {
        TestCase *tc = malloc(sizeof(TestCase));
        tc->description = desc;
        tc->func = func;
        tc->next = NULL;
        if (TESTS_TAIL)
                TESTS_TAIL->next = tc;
        else
                TESTS_HEAD = tc;
        TESTS_TAIL = tc;
}

void tunit_assert(int expr, const char *expr_str, const char *msg,
                  const char *file, int line) {
        if (!expr) {
                fprintf(stderr,
                        "  [FAIL] Assertion failed: %s in test %s: %s at %s:%d\n",
                        msg, CURRENT_TEST ? CURRENT_TEST : "(unknown)",
                        expr_str, file, line);
                TESTS_FAIL++;
                longjmp(JMP_BUFFER, 1);
        }
}

void tunit_skip_test(const char *reason) {
        fprintf(stderr, "  [SKIP] Test '%s' skipped: %s\n", CURRENT_TEST,
                reason);
        TESTS_SKIP++;
        longjmp(JMP_BUFFER, 2);
}

void tunit_run_all_tests(void) {
        struct timespec suite_start;
        clock_gettime(CLOCK_MONOTONIC, &suite_start);

        TestCase *curr = TESTS_HEAD;
        while (curr) {
                TESTS_RUN++;
                run_single_test(curr);
                curr = curr->next;
        }

        double suite_duration_ms = get_time_diff_ms(&suite_start);
        fprintf(stderr, " === Test Summary ===\n");
        fprintf(stderr, "%d succeed, %d failed and %d skipped (%.2fms total)\n",
                TESTS_RUN, TESTS_FAIL, TESTS_SKIP, suite_duration_ms);
}

static inline void tunit_init_log(const char *log_path) {
        TUNIT_LOG_FILE = fopen(log_path, "a");
        if (!TUNIT_LOG_FILE) {
                perror("failed to open log file");
        }
        tunit_fd = fileno(TUNIT_LOG_FILE);
}

void tunit_log(const char *fmt, ...) {
        if (!TUNIT_LOG_FILE) return;

        va_list args;
        va_start(args, fmt);
        fprintf(TUNIT_LOG_FILE, "%s\n", CURRENT_TEST);
        vfprintf(TUNIT_LOG_FILE, fmt, args);
        va_end(args);
        fflush(TUNIT_LOG_FILE);
}

static inline void _tunit_log(const char *msg) {
        if (!TUNIT_LOG_FILE) return;

        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        char buffer[100];

        strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", t);

        fprintf(TUNIT_LOG_FILE,
                "===============================================\n=== %s - %s ===\n===============================================\n",
                msg, buffer);
        fflush(TUNIT_LOG_FILE);
}

static inline void tunit_close_log(void) {
        if (TUNIT_LOG_FILE) fclose(TUNIT_LOG_FILE);
        if (tunit_fd != -1) close(tunit_fd);
}

int main(void) {
        srand((unsigned)time(NULL));
        char perturb[16];
        snprintf(perturb, sizeof(perturb), "%d", (rand() % 255) + 1);
        setenv("MALLOC_PERTURB_", perturb, 1);

        tunit_init_log("tunit.log");
        _tunit_log("Initializing test");
        tunit_run_all_tests();
        _tunit_log("Finished test");
        tunit_close_log();
        return TESTS_FAIL;
}

#endif
