#include <sys/cdefs.h>
#define AOCLIBS_IMPLEMENTATION
#ifdef AOCLIBS_IMPLEMENTATION

#ifndef AOCLIBS_H
#define AOCLIBS_H

/*
 * AOCLIBS - Augusto Coronel's C Libraries
 *
 * This library is hosted on Codeberg at https://codeberg.org/aocoronel/aoclibs,
 * and is licensed under the Unlicense License (Public Domain).
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
 * software, either in source code form or as a compiled binary, for any purpose,
 * commercial or non-commercial, and by any means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors of this
 * software dedicate any and all copyright interest in the software to the public
 * domain. We make this dedication for the benefit of the public at large and to
 * the detriment of our heirs and successors. We intend this dedication to be an
 * overt act of relinquishment in perpetuity of all present and future rights to
 * this software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <https://unlicense.org/>
*/

// === Config ===

#define AOC_CLI_BUFFER 128
#define DIR_WALKER_BUFF 4096

#define PRINTH_DESC_INDENT 10 // Indentation by spaces

#define FEATURE_DISABLE_COLORS // Disable colors

#define AOCLIBS_ALLOC // Memory allocation implementation
#define AOCLIBS_ARENA // Arena implementation
#define AOCLIBS_CLI // CLI implementation
#define AOCLIBS_DEBUG // Debug utilities
#define AOCLIBS_FILE // File handling implementation
#define AOCLIBS_STRING // String implementation
#define AOCLIBS_VEC // Dynamic arrays implementation

// Enable TUnit :: cc -DTUNIT
// Enable Heap Debugger :: cc -DDEBUG_HEAP

// === Includes ===

#ifdef TUNIT
#define _XOPEN_SOURCE 600
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <unistd.h>
#endif

#ifdef DEBUG_HEAP
#include <stdio.h>
#include <stdlib.h>
#endif

#ifdef AOCLIBS_ALLOC
#include <stdlib.h>
#endif

#ifdef AOCLIBS_STRING
#include <alloca.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

#ifdef AOCLIBS_DEBUG
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#endif

#ifdef AOCLIBS_ARENA
#include <errno.h>
#include <limits.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#endif

#ifdef AOCLIBS_VEC
#include <errno.h>
#include <stdalign.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

#ifdef AOCLIBS_FILE
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#endif

#ifdef AOCLIBS_CLI
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#endif

#include <stdint.h>

// === Attributes ===

/*
 * ref :: is a reference and cannot be NULL
 * null :: can be NULL
*/

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

/*
 * Indicate a function that can abort the program
*/
#define FN_ABORTS

#define UNLIKELY(x) __builtin_expect(!!(x), 0)

#define FN_COLD __attribute__((cold))
#define FN_CONST __attribute__((const))
#define FN_DEPRECATED(x) __attribute_deprecated_msg__(x)
#define FN_HEAP_ALLOCATES __attribute__((malloc))
#define FN_HOT __attribute__((hot))
#define FN_NONNULL_RETURN __attribute__((returns_nonnull))
#define FN_NORETURN __attribute__((noreturn))
#define FN_PACKED __attribute__((__packed__))
#define FN_PURE __attribute__((pure))
#define FN_WARN_UNUSED __attribute__((warn_unused_result))

// === Macros ===

#define shift(s) ((s)++)
#define shiftn(s, i) ((s) += (i))

#define string_literal_len(s) ((sizeof((s)) / sizeof((s)[0])) - sizeof((s)[0]))
#define array_len(a) sizeof(a) / sizeof(a[0])

/*
 * Asserts an expression, and prints a formatted message
*/
#ifdef NDEBUG
#define ASSERT(...)
#else
#define ASSERT(exp, ...) \
        (void)((!!(exp)) || (aoc_assert(#exp, __FILE__, __LINE__, __func__, __VA_ARGS__), 0))
#endif

#define ASSERT_REF(exp) ASSERT((exp), "passing NULL pointer to Nonnull parameter")

/*
 * Used to panic, when an unreachable code is ran
*/
#define not_reachable() aoc_panic(__FILE__, __LINE__, __func__, "unreachable code reached")

/*
 * Prints message, including metadata and aborts the program
*/
#define panic(msg) aoc_panic(__FILE__, __LINE__, __func__, msg)

/*
 * Print debug/trace formatted message to stderr
*/
#ifdef NDEBUG
#define PDBG(...) ((void)0)
#define PTRACE(...) ((void)0)
#else
#define PDBG(...) aoc_printfc_debug(__VA_ARGS__)
#define PTRACE(...) aoc_printfc_trace(__VA_ARGS__)
#endif // NDEBUG

/*
 * TEST(my_test_name) :: Register a test
 * TASSERT(1 == 1) :: TUnit specific asserts
 * SKIP_TEST(my_test_name) :: Skip registering test
*/

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

#define SKIP_TEST(reason)                \
        do {                             \
                tunit_skip_test(reason); \
                return;                  \
        } while (0)
#endif

// === Features ===

/*
 * TTY Colors
*/
#ifdef FEATURE_DISABLE_COLORS
#define COLOR_RESET ""

#define COLOR_BOLD ""
#define COLOR_UNDERLINE ""
#define COLOR_BOLD_UNDERLINE ""

#define COLOR_BLACK ""
#define COLOR_RED ""
#define COLOR_GREEN ""
#define COLOR_YELLOW ""
#define COLOR_BLUE ""
#define COLOR_MAGENTA ""
#define COLOR_CYAN ""
#define COLOR_WHITE ""

#define COLOR_BLACK_BRIGHT ""
#define COLOR_RED_BRIGHT ""
#define COLOR_GREEN_BRIGHT ""
#define COLOR_YELLOW_BRIGHT ""
#define COLOR_BLUE_BRIGHT ""
#define COLOR_MAGENTA_BRIGHT ""
#define COLOR_CYAN_BRIGHT ""
#define COLOR_WHITE_BRIGHT ""
#else
#define COLOR_RESET "\x1b[0m"

#define COLOR_BOLD "\x1b[1m"
#define COLOR_UNDERLINE "\x1b[4m"
#define COLOR_BOLD_UNDERLINE "\x1b[1;4m"

#define COLOR_BLACK "\x1b[30m"
#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_BLUE "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN "\x1b[36m"
#define COLOR_WHITE "\x1b[37m"

#define COLOR_BLACK_BRIGHT "\x1b[90m"
#define COLOR_RED_BRIGHT "\x1b[91m"
#define COLOR_GREEN_BRIGHT "\x1b[92m"
#define COLOR_YELLOW_BRIGHT "\x1b[93m"
#define COLOR_BLUE_BRIGHT "\x1b[94m"
#define COLOR_MAGENTA_BRIGHT "\x1b[95m"
#define COLOR_CYAN_BRIGHT "\x1b[96m"
#define COLOR_WHITE_BRIGHT "\x1b[97m"
#endif

#define aoc_cli_program(cmds, arguments, options, env) \
        { .flagc = array_len(options),                 \
          .cmdc = array_len(cmds),                     \
          .argc = array_len(arguments),                \
          .name = PROGRAM_NAME,                        \
          .desc = PROGRAM_DESC,                        \
          .usage = PROGRAM_USAGE,                      \
          .commands = (cmds),                          \
          .args = (arguments),                         \
          .flags = (options) }

// #define AOCLIBS_FN_PREFIX aoc_

// === Types ===

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef intptr_t isize;
typedef uintptr_t usize;

/*
 * Error Enum
 * An external file "errno.h" may be supplied to generate all errors in this
 * enum
*/
enum {
        Ok = 0,
#define E(name) name,
#include "__errno.h"
#undef E
} FN_PACKED;

// === Function Declarations ===

// Error Handling && Debugging

#ifdef AOCLIBS_DEBUG
/*
 * Asserts an expression, and prints a formatted message
*/
void aoc_assert(const char *expr, const char *file, unsigned line, const char *func,
                const char *fmt, ...) FN_ABORTS;

/*
 * Prints message, including metadata and aborts the program
 *
 * Should not be used directly. Use the panic macro, instead.
*/
void aoc_panic(const char *__file, int __line, const char *__func, const char *msg) FN_ABORTS;

/*
 * Returns a dot spinner symbol.
 *
 * Using this function multiple times, gives the next spinner.
*/
const char *aoc_get_spinner(void);

/*
 * Prints indented message
 *
 * If the size of the terminal cannot be obtained, fallback to 80 columns
*/
void aoc_iprint(const char *msg, i32 indent);

/*
 * Print fatal formatted message to stderr
 *
 * Defines:
 * - FEATURE_DISABLE_COLORS :: Disable colors
*/
void aoc_printfc_fatal(const char *ref fmt, ...);

/*
 * Print error formatted message to stderr
 *
 * Defines:
 * - FEATURE_DISABLE_COLORS :: Disable colors
*/
void aoc_printfc_error(const char *ref fmt, ...);

/*
 * Print warning formatted message to stderr
 *
 * Defines:
 * - FEATURE_DISABLE_COLORS :: Disable colors
*/
void aoc_printfc_warn(const char *ref fmt, ...);

/*
 * Print info formatted message to stderr
 *
 * Defines:
 * - FEATURE_DISABLE_COLORS :: Disable colors
*/
void aoc_printfc_info(const char *ref fmt, ...);

/*
 * Print debug formatted message to stderr
 *
 * Defines:
 * - FEATURE_DISABLE_COLORS :: Disable colors
*/
void aoc_printfc_debug(const char *ref fmt, ...);

/*
 * Print trace formatted message to stderr
 *
 * Defines:
 * - FEATURE_DISABLE_COLORS :: Disable colors
*/
void aoc_printfc_trace(const char *ref fmt, ...);

/*
 * Print critical formatted message to stderr
 *
 * Defines:
 * - FEATURE_DISABLE_COLORS :: Disable colors
*/
void aoc_printfc_critical(const char *ref fmt, ...);
#endif

// Heap Debugger

#ifdef DEBUG_HEAP
/*
 * Prints allocation and free count.
 * If there was a leak, print the source of the leak
*/
void debug_memory_summary(FILE *ref fd);

/*
 * Returns how many leaks found
*/
int debug_count_leaks(void);
#endif /* DEBUG_HEAP */

// File Handling

#ifdef AOCLIBS_FILE
typedef enum FileType {
        F_NULL, // Doesn't exist
        F_REG, // Regular file
        F_DIR, // Directory
        F_LNK, // Symbolic Link
} FileType;

typedef void (*dw_fn)(const char *path);

/*
 * Walks into a directory and read it's content
 *
 * aoc_dir_walk can run four user provided functions based on each filetype:
 * reg, directory, lnk and null.
 *
 * If the user prefers to ignore a certain filetype, the function values can be
 * passed as NULL.
 *
 * When recurse is set to true, when a directory is found, aoc_dir_walk will recurse
 * in it.
 *
 * Defines:
 * - NDEBUG :: Disable asserts
 *
 * Failure: errno << opendir
*/
int aoc_dir_walk(const char *ref path, bool recurse, dw_fn isdir, dw_fn isreg, dw_fn islnk,
                 dw_fn isnull, dw_fn isempty);

/*
 * Stat the file and return its type
 *
 * Defines:
 * - NDEBUG :: Disable asserts
*/
FileType aoc_get_filetype(const char *ref path);
#endif

// Arena

#ifdef AOCLIBS_ARENA
#define AOC_ARENA_DEFAULT_ALIGNMENT (alignof(void *))

typedef struct Arena {
        u8 *buffer;
        usize cap;
        usize offset;
} Arena;

Arena aoc_arena_create(usize cap);
void aoc_arena_reset(Arena *ref a);
void aoc_arena_destroy(Arena *ref a);
void *aoc_arena_alloc_aligned(Arena *ref a, usize size, usize align);
void *aoc_arena_alloc(Arena *ref a, usize size);
char *aoc_arena_alloc_chars(Arena *ref a, usize count);
#endif

// Dynamic Array (a.k.a Vector)

#ifdef AOCLIBS_VEC
typedef int (*VecCompare)(const void *a, const void *b);

typedef struct {
        void *data;
        usize len;
        usize cap;
        usize pad; // Size of each element in bytes
} Vec;

Vec aoc_vec_init(usize pad);
void aoc_vec_free(Vec *ref v);
int aoc_vec_reserve(Vec *ref v, usize cap) FN_WARN_UNUSED;
int aoc_vec_realloc(Vec *ref v) FN_WARN_UNUSED;
void *aoc_vec_at(Vec *ref v, usize index) FN_WARN_UNUSED;
int aoc_vec_push(Vec *ref v, const void *value) FN_WARN_UNUSED;
int aoc_vec_pop(Vec *ref v, void *ref output) FN_WARN_UNUSED;
int aoc_vec_insert(Vec *ref v, usize index, const void *null value) FN_WARN_UNUSED;
void aoc_vec_swap(Vec *ref v, usize a, usize b);
usize aoc_vec_partition(Vec *ref v, VecCompare cmp, usize lo, usize hi);
void aoc_vec_qsort(Vec *ref v, VecCompare cmp, usize lo, usize hi);
void aoc_vec_sort(Vec *ref v, VecCompare cmp);
int aoc_vec_shrink_to_fit(Vec *ref v) FN_WARN_UNUSED;
void aoc_vec_clear(Vec *ref v);
int aoc_vec_erase(Vec *v, usize index) FN_WARN_UNUSED;
int aoc_vec_resize(Vec *ref v, usize new_len, const void *null value) FN_WARN_UNUSED;
int aoc_vec_copy(Vec *xref dest, Vec *xref src) FN_WARN_UNUSED;
int aoc_vec_equal(Vec *a, Vec *b, VecCompare cmp);
void aoc_vec_for_each(Vec *v, void (*fn)(void *));
#endif

// CLI Helpers

#ifdef AOCLIBS_CLI
enum CLIArgumentType {
        ReqArg, // Required Argument
        NReqArg, // Not Required Argument
};

typedef struct {
        const char *name;
        const char *desc;
        const char *completion;
        enum CLIArgumentType type;
} CLIArgument;

typedef struct {
        const char *cmd;
        const CLIArgument *args;
        const char *desc;
} CLICommand;

typedef struct {
        const char *short_opt;
        const char *long_opt;
        const CLIArgument *args;
        const char *desc;
} CLIOption;

typedef struct {
        const char *name;
        const char *value; // HOME, PATH, SHELL...
} CLIEnv;

typedef struct {
        usize flagc; // Flag count
        usize cmdc; // Command count
        usize argc; // Argument count
        const char *name;
        const char *desc;
        const char *usage;
        CLIArgument *args;
        CLICommand *commands;
        CLIOption *flags;
} CLIProgram;

/*
 * Generates bash completions for commands, flags and arguments
*/
void _aoc_bashgen(CLIProgram prog, const CLIEnv *env, usize envc);

/*
 * Normalizes a strstrg strto a valid shell function name, by replacstrg
 * non-alphanumeric characters with underscores.
 *
 * May truncate to buff_size
*/
void cli_normalize_shell_name(char *buff, const char *str, usize buff_size);

/*
 * Prints help message
*/
void _aoc_printh(CLIProgram prog);

/*
 * Generates bash completions for commands, flags and arguments
*/
void _aoc_zshgen(CLIProgram prog, const CLIEnv *env, usize envc);
#endif

// TUnit -- Test Unit

#ifdef TUNIT
/*
 * Logs formatted message to tunit.txt
*/
void tunit_log(const char *fmt, ...);
#endif

// === Function Definitions ===

#ifdef AOCLIBS_DEBUG

void aoc_assert(const char *expr, const char *file, unsigned line, const char *func,
                const char *fmt, ...) FN_ABORTS {
        va_list args;
        va_start(args, fmt);
        fprintf(stderr, "Assertion failed: ");
        vfprintf(stderr, fmt, args);
        fprintf(stderr, "\n%s at %s:%u (%s)\n", expr, file, line, func);
        va_end(args);

        abort();
}

void aoc_panic(const char *__file, int __line, const char *__func, const char *msg) FN_ABORTS {
        fprintf(stderr, "PANIC: %s at %s:%d (%s) ", msg, __file, __line, __func);
        fflush(stderr);
        abort();
}

void aoc_printfc_fatal(const char *ref fmt, ...) {
        va_list args;
        va_start(args, fmt);
        fprintf(stderr, "%s[%s]:%s ", COLOR_RED, "FATAL", COLOR_RESET);
        vfprintf(stderr, fmt, args);
        va_end(args);
}

void aoc_printfc_error(const char *ref fmt, ...) {
        va_list args;
        va_start(args, fmt);
        fprintf(stderr, "%s[%s]:%s ", COLOR_RED, "ERROR", COLOR_RESET);
        vfprintf(stderr, fmt, args);
        va_end(args);
}

void aoc_printfc_warn(const char *ref fmt, ...) {
        va_list args;
        va_start(args, fmt);
        fprintf(stderr, "%s[%s]:%s ", COLOR_YELLOW, "WARNING", COLOR_RESET);
        vfprintf(stderr, fmt, args);
        va_end(args);
}

void aoc_printfc_info(const char *ref fmt, ...) {
        va_list args;
        va_start(args, fmt);
        fprintf(stderr, "%s[%s]:%s ", COLOR_GREEN, "INFO", COLOR_RESET);
        vfprintf(stderr, fmt, args);
        va_end(args);
}

void aoc_printfc_debug(const char *ref fmt, ...) {
        va_list args;
        va_start(args, fmt);
        fprintf(stderr, "%s[%s]:%s ", COLOR_CYAN, "DEBUG", COLOR_RESET);
        vfprintf(stderr, fmt, args);
        va_end(args);
}

void aoc_printfc_trace(const char *ref fmt, ...) {
        va_list args;
        va_start(args, fmt);
        fprintf(stderr, "%s[%s]:%s ", COLOR_MAGENTA, "TRACE", COLOR_RESET);
        vfprintf(stderr, fmt, args);
        va_end(args);
}

void aoc_printfc_critical(const char *ref fmt, ...) {
        va_list args;
        va_start(args, fmt);
        fprintf(stderr, "%s[%s]:%s ", COLOR_RED, "CRITICAL", COLOR_RESET);
        vfprintf(stderr, fmt, args);
        va_end(args);
}

#define SPINNER_NUM_SYMBOLS (sizeof(SYMBOLS) / sizeof(SYMBOLS[0]))

const char *aoc_get_spinner(void) {
        static const char *SYMBOLS[] = { "⠁", "⠈", "⠐", "⠠", "⢀", "⡀", "⠄", "⠂" };
        static int_fast8_t counter = 0;
        const char *RESULT = SYMBOLS[counter];
        counter = (counter + 1) % SPINNER_NUM_SYMBOLS;
        return RESULT;
}

void aoc_iprint(const char *msg, i32 indent) {
        struct winsize w;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1 || w.ws_col == 0) {
                w.ws_col = 80;
        }
        const i32 WIDTH = w.ws_col;

        fprintf(stderr, "%-*s", indent, "");
        int line_pos = indent;

        const char *START = msg;
        const char *END = msg;

        while (*END) {
                if (*END == '\n') {
                        fputc('\n', stderr);
                        fprintf(stderr, "%*s", indent, "");
                        line_pos = indent;
                        END++;
                        continue;
                }
                while (isspace((unsigned char)*END))
                        END++;

                if (*END == ' ') break;

                START = END;
                while (*END && !isspace((unsigned char)*END))
                        END++;

                int word_len = END - START;

                if (line_pos + word_len > WIDTH && line_pos > indent) {
                        fprintf(stderr, "\n%*s", indent, "");
                        line_pos = indent;
                }

                fprintf(stderr, "%.*s", word_len, START);
                line_pos += word_len;

                if (*END && line_pos < WIDTH) {
                        fputc(' ', stderr);
                        line_pos++;
                }
        }
        fputc(' ', stderr);
}
#endif

#ifdef TUNIT
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
        fprintf(stderr, "  [TIMEOUT] Test '%s' exceeded time limit (%d)\n", CURRENT_TEST, sig);
        TESTS_FAIL++;
        longjmp(JMP_BUFFER, 1);
}

void segfault_handler(int sig) {
        fprintf(stderr, "  [CRASH] SEGFAULT (signal %d) in test '%s'\n", sig, CURRENT_TEST);
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
                                fprintf(stderr, " ✓ %s %.2fms\n", CURRENT_TEST, duration_ms);
                        } else if (strcmp(result, "SKIP") == 0) {
                                clock_gettime(CLOCK_MONOTONIC, &end);
                                double duration_ms = get_time_diff_ms(&start);
                                fprintf(stderr, " s %s %.2fms\n", CURRENT_TEST, duration_ms);
                                TESTS_SKIP++;
                        }
                } else {
                        clock_gettime(CLOCK_MONOTONIC, &end);
                        double duration_ms = get_time_diff_ms(&start);
                        if (WIFSIGNALED(status)) {
                                fprintf(stderr, " ✗ %s %.2fms (CRASH: signal %d)\n", CURRENT_TEST,
                                        duration_ms, WTERMSIG(status));
                        } else {
                                fprintf(stderr, " ✗ %s %.2fms (UNKNOWN ERROR)\n", CURRENT_TEST,
                                        duration_ms);
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

void tunit_assert(int expr, const char *expr_str, const char *msg, const char *file, int line) {
        if (!expr) {
                fprintf(stderr, "  [FAIL] Assertion failed: %s in test %s: %s at %s:%d\n", msg,
                        CURRENT_TEST ? CURRENT_TEST : "(unknown)", expr_str, file, line);
                TESTS_FAIL++;
                longjmp(JMP_BUFFER, 1);
        }
}

void tunit_skip_test(const char *reason) {
        fprintf(stderr, "  [SKIP] Test '%s' skipped: %s\n", CURRENT_TEST, reason);
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
        fprintf(stderr, "%d succeed, %d failed and %d skipped (%.2fms total)\n", TESTS_RUN,
                TESTS_FAIL, TESTS_SKIP, suite_duration_ms);
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

#ifdef AOCLIBS_ARENA

Arena aoc_arena_create(usize cap) {
        Arena a = { 0 };
        if (cap == 0) return a;

        a.buffer = malloc(cap);
        if (!a.buffer) return a;

        a.cap = cap;
        a.offset = 0;

        return a;
}

void aoc_arena_reset(Arena *ref a) {
        ASSERT_REF(a != NULL);
        a->offset = 0;
}

void aoc_arena_destroy(Arena *ref a) {
        ASSERT(a != NULL, "%s", "double free attempt");
        free(a->buffer);
        a->buffer = NULL;
        a->cap = 0;
        a->offset = 0;
}

void *aoc_arena_alloc_aligned(Arena *ref a, usize size, usize align) {
        ASSERT(a != NULL, "%s", "double free attempt");
        ASSERT((align & (align - 1)) == 0, "%s", "alignment is not a power of two");

        usize curr = (usize)(a->buffer + a->offset);
        usize misalignment = curr & (align - 1);
        usize padding = misalignment ? (align - misalignment) : 0;

        if (size > SIZE_MAX - a->offset - padding) return NULL;

        usize needed = a->offset + padding + size;

        if (needed > a->cap) {
                usize new_cap = a->cap ? a->cap * 2 : align;

                if (new_cap < needed) new_cap = needed;

                u8 *new_buf = realloc(a->buffer, new_cap);
                if (!new_buf) return NULL;

                a->buffer = new_buf;
                a->cap = new_cap;
        }

        a->offset += padding;
        void *result = a->buffer + a->offset;
        a->offset += size;

        return result;
}

void *aoc_arena_alloc(Arena *ref a, usize size) {
        return aoc_arena_alloc_aligned(a, size, AOC_ARENA_DEFAULT_ALIGNMENT);
}

char *aoc_arena_alloc_chars(Arena *ref a, usize count) {
        return (char *)aoc_arena_alloc_aligned(a, count, 1);
}
#endif

#ifdef AOCLIBS_VEC

Vec aoc_vec_init(usize pad) {
        return (Vec){ .data = NULL, .len = 0, .cap = 0, .pad = pad };
}

void aoc_vec_free(Vec *ref v) {
        ASSERT(v != NULL, "%s", "double free attempt");
        free(v->data);
        *v = (Vec){ 0 };
}

int aoc_vec_reserve(Vec *ref v, usize cap) {
        ASSERT_REF(v != NULL);

        if (cap > SIZE_MAX / v->pad) return MaxCapacityReached;

        void *tmp_ptr = realloc(v->data, cap * v->pad);
        if (!tmp_ptr) return errno;

        void *new_data = tmp_ptr;

        v->data = new_data;
        v->cap = cap;
        return Ok;
}

int aoc_vec_realloc(Vec *ref v) {
        ASSERT_REF(v != NULL);

        usize new_cap = v->cap ? v->cap * 2 : 8;

        if (new_cap > SIZE_MAX / v->pad) return MaxCapacityReached;

        void *new_data = realloc(v->data, new_cap * v->pad);
        if (!new_data) return errno;

        v->data = new_data;
        v->cap = new_cap;
        return Ok;
}

void *aoc_vec_at(Vec *ref v, usize index) {
        ASSERT_REF(v != NULL);
        if (index > v->len) return NULL;

        return (char *)v->data + index * v->pad;
}

int aoc_vec_push(Vec *ref v, const void *value) {
        ASSERT_REF(v != NULL);
        int err = aoc_vec_reserve(v, v->len + 1);
        if (err != Ok) return err;
        char *dest = (char *)v->data + v->len * v->pad;
        memcpy(dest, value, v->pad);
        v->len++;
        return Ok;
}

int aoc_vec_pop(Vec *ref v, void *ref output) {
        ASSERT_REF(v != NULL);
        if (v->len == 0) return ElementIsEmpty;
        v->len--;
        if (output) {
                memcpy(output, (char *)v->data + v->len * v->pad, v->pad);
        }
        return Ok;
}

int aoc_vec_insert(Vec *ref v, usize index, const void *null value) {
        ASSERT_REF(v != NULL);
        if (index > v->len) return IndexNoExist;
        int err = aoc_vec_reserve(v, v->len + 1);
        if (err != Ok) return err;

        void *dst = (char *)v->data + (index + 1) * v->pad;
        void *src = (char *)v->data + index * v->pad;
        usize move = (v->len - index) * v->pad;

        memmove(dst, src, move);
        memcpy(src, value, v->pad);
        v->len++;
        return Ok;
}

void aoc_vec_swap(Vec *ref v, usize a, usize b) {
        ASSERT_REF(v != NULL);
        if (a == b) return;

        char *pa = (char *)v->data + a * v->pad;
        char *pb = (char *)v->data + b * v->pad;
        char *tmp = malloc(v->pad);
        if (!tmp) return;

        memcpy(tmp, pa, v->pad);
        memcpy(pa, pb, v->pad);
        memcpy(pb, tmp, v->pad);
        free(tmp);
}

usize aoc_vec_partition(Vec *ref v, VecCompare cmp, usize lo, usize hi) {
        ASSERT_REF(v != NULL);
        void *err = aoc_vec_at(v, hi);
        if (err == NULL) return ValueNotFound;

        void *pivot = err;

        usize i = lo;

        for (usize j = lo; j < hi; j++) {
                err = aoc_vec_at(v, j);
                if (err != Ok) return -1;
                void *aoc_vec_data = err;
                if (cmp(aoc_vec_data, pivot) < 0) {
                        aoc_vec_swap(v, i, j);
                        i++;
                }
        }

        aoc_vec_swap(v, i, hi);
        return i;
}

void aoc_vec_qsort(Vec *ref v, VecCompare cmp, usize lo, usize hi) {
        ASSERT_REF(v != NULL);
        if (lo >= hi) return;

        usize p = aoc_vec_partition(v, cmp, lo, hi);

        if (p > 0) aoc_vec_qsort(v, cmp, lo, p - 1);
        aoc_vec_qsort(v, cmp, p + 1, hi);
}

void aoc_vec_sort(Vec *ref v, VecCompare cmp) {
        ASSERT_REF(v != NULL);
        if (v->len > 1) aoc_vec_qsort(v, cmp, 0, v->len - 1);
}

int aoc_vec_shrink_to_fit(Vec *ref v) {
        ASSERT_REF(v != NULL);
        if (v->len >= v->cap) return NothingToDo;
        void *new_data = realloc(v->data, v->len * v->pad);
        if (!new_data) return errno;
        v->data = new_data;
        v->cap = v->len;
        return Ok;
}

void aoc_vec_clear(Vec *ref v) {
        ASSERT_REF(v != NULL);
        v->len = 0;
}

int aoc_vec_erase(Vec *v, usize index) {
        ASSERT_REF(v != NULL);
        if (index > v->len) return IndexNoExist;
        void *dst = (char *)v->data + index * v->pad;
        void *src = (char *)v->data + (index + 1) * v->pad;
        usize move = (v->len - index - 1) * v->pad;
        memmove(dst, src, move);
        v->len--;
        return Ok;
}

int aoc_vec_resize(Vec *ref v, usize new_len, const void *null value) {
        ASSERT_REF(v != NULL);
        if (new_len > v->len) {
                int err = aoc_vec_reserve(v, new_len);
                if (err != Ok) return err;
                for (usize i = v->len; i < new_len; i++)
                        memcpy((char *)v->data + i * v->pad, value, v->pad);
        }
        v->len = new_len;
        return Ok;
}

int aoc_vec_copy(Vec *xref dest, Vec *xref src) {
        if (dest->pad != src->pad) return IncompatibleTypes;
        int err = aoc_vec_reserve(dest, src->len);
        if (err == Ok) return err;
        memcpy(dest->data, src->data, src->len * src->pad);
        dest->len = src->len;
        return Ok;
}

int aoc_vec_equal(Vec *a, Vec *b, VecCompare cmp) {
        if (a->len != b->len || a->pad != b->pad) return -1;
        for (usize i = 0; i < a->len; i++) {
                void *err_1 = aoc_vec_at(a, i);
                if (err_1 == NULL) return -1;

                void *err_2 = aoc_vec_at(b, i);
                if (err_2 == NULL) return -1;

                if (cmp(err_1, err_2) != 0) return -1;
        }
        return 0;
}

void aoc_vec_for_each(Vec *v, void (*fn)(void *)) {
        for (usize i = 0; i < v->len; i++) {
                void *err = aoc_vec_at(v, i);
                if (err != NULL) fn(err);
        }
}
#endif

#ifdef AOCLIBS_FILE
int aoc_dir_walk(const char *ref path, bool recurse, dw_fn isdir, dw_fn isreg, dw_fn islnk,
                 dw_fn isnull, dw_fn isempty) {
        ASSERT_REF(path != NULL);

        DIR *dir = opendir(path);
        if (!dir) return errno;

        u8 empty = 0;

        struct dirent *entry;
        char fullpath[DIR_WALKER_BUFF];

        while ((entry = readdir(dir)) != NULL) {
                if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

                snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

                FileType file_t = aoc_get_filetype(fullpath);

                switch (file_t) {
                case F_REG:
                        if (isreg != NULL) isreg(fullpath);
                        break;
                case F_DIR:
                        if (recurse)
                                aoc_dir_walk(fullpath, recurse, isdir, isreg, islnk, isnull,
                                             isempty);
                        if (isdir != NULL) isdir(fullpath);
                        break;
                case F_LNK:
                        if (islnk != NULL) islnk(fullpath);
                        break;
                case F_NULL:
                        if (isnull != NULL) isnull(fullpath);
                        break;
                default:
                        break;
                }

                empty++;
        }

        if (empty == 0 && isempty != NULL) isempty(path);

        closedir(dir);
        return Ok;
}

FileType aoc_get_filetype(const char *ref path) {
        ASSERT_REF(path != NULL);

        struct stat st;
        if (lstat(path, &st) == -1) return F_NULL;

        if (S_ISREG(st.st_mode))
                return F_REG;
        else if (S_ISDIR(st.st_mode))
                return F_DIR;
        else if (S_ISLNK(st.st_mode))
                return F_LNK;

        return F_NULL;
}
#endif

#ifdef AOCLIBS_CLI
static inline void bashgen_shebang(void) {
        puts("#!/usr/bin/env bash");
}

static inline void bashgen_env(const CLIEnv *env, usize envc) {
        for (usize i = 0; i < envc; i++)
                printf("%s=%s\n", env[i].name, env[i].value);
}

static inline void bashgen_argument(const CLIArgument *args) {
        char ARG[AOC_CLI_BUFFER];
        if (args && args->completion)
                cli_normalize_shell_name(ARG, args->name, AOC_CLI_BUFFER);
        else
                return;
        printf("_%s() {\n  %s\n}\n", ARG, args->completion);
}

static inline void bashgen_flags(const CLIProgram *prog) {
        putchar(' ');
        for (usize i = 0; i < prog->flagc; i++) {
                if (prog->flags[i].long_opt != NULL) printf(" %s", prog->flags[i].long_opt);
                if (prog->flags[i].short_opt != NULL) printf(" %s", prog->flags[i].short_opt);
        }
}

static inline void bashgen_commands(const CLIProgram *prog) {
        for (usize i = 0; i < prog->cmdc; i++)
                printf(" %s", prog->commands[i].cmd);
}

static inline void bashgen_flag_cases(const CLIProgram *prog, const CLIOption *flags) {
        char ARG[AOC_CLI_BUFFER];
        if (flags->args && flags->args->completion)
                cli_normalize_shell_name(ARG, prog->args->name, AOC_CLI_BUFFER);
        else
                return;
        const char *SHORT_FLAG = flags->short_opt;
        const char *LONG_FLAG = flags->long_opt;
        if (LONG_FLAG && SHORT_FLAG)
                printf("  %s|%s)\n", SHORT_FLAG, LONG_FLAG);
        else if (LONG_FLAG)
                printf("  %s)\n", LONG_FLAG);
        else if (SHORT_FLAG)
                printf("  %s)\n", SHORT_FLAG);
        else
                return;
        printf("    mapfile -t COMPREPLY < <(compgen -W \"$(_%s)\" -- \"${cur}\")\n", ARG);
        printf("    return 0\n    ;;\n");
}

static inline void bashgen_command_cases(const CLIProgram *prog, const CLICommand *commands) {
        char ARG[AOC_CLI_BUFFER];
        if (commands->args && commands->args->completion)
                cli_normalize_shell_name(ARG, prog->args->name, AOC_CLI_BUFFER);
        else
                return;
        printf("  %s)\n", commands->cmd);
        printf("    mapfile -t COMPREPLY < <(compgen -W \"$(_%s)\" -- \"${cur}\")\n", ARG);
        printf("    return 0\n    ;;\n");
}

void _aoc_bashgen(const CLIProgram prog, const CLIEnv *env, usize envc) {
        bashgen_shebang();
        bashgen_env(env, envc);
        for (usize i = 0; i < prog.argc; i++) {
                bashgen_argument(&prog.args[i]);
        }

        // Main function
        printf("_%s() {\n", prog.name);
        printf("  local cur prev\n");
        printf("  cur=\"${COMP_WORDS[COMP_CWORD]}\"\n");
        printf("  prev=\"${COMP_WORDS[COMP_CWORD-1]}\"\n");
        printf("  COMPREPLY=()\n");

        // Flag completion
        printf("  if [[ \"${cur}\" == -* ]]; then\n");
        printf("    mapfile -t COMPREPLY < <(compgen -W \"");
        bashgen_flags(&prog);
        printf("\" -- \"${cur}\")\n");
        printf("    return 0\n");
        printf("  fi\n");

        // Argument completion
        printf("  case \"${prev}\" in\n");
        for (usize i = 0; i < prog.cmdc; i++) {
                bashgen_command_cases(&prog, &prog.commands[i]);
        }
        for (usize i = 0; i < prog.flagc; i++) {
                bashgen_flag_cases(&prog, &prog.flags[i]);
        }
        printf("  esac\n");

        // Command completion
        printf("  mapfile -t COMPREPLY < <(compgen -W \"");
        bashgen_commands(&prog);
        printf("\" -- \"${cur}\")\n");
        printf("  return 0\n");
        printf("}\n");

        // Assign function to program
        printf("complete -F _%s %s\n", prog.name, prog.name);
}

void cli_normalize_shell_name(char *buff, const char *str, usize buff_size) {
        if (!buff || !str || buff_size == 0) {
                if (buff && buff_size > 0) buff[0] = '\0';
                return;
        }

        usize i = 0;

        for (; *str && i < buff_size; str++) {
                if (isalnum(*str) || *str == '_') {
                        buff[i++] = *str;
                } else {
                        buff[i++] = '_';
                }
        }

        if (i < buff_size)
                buff[i] = '\0';
        else
                buff[buff_size - 1] = '\0';

        if (i > 0 && isdigit(buff[0])) {
                memmove(buff + 1, buff, i);
                buff[0] = '_';
        }
}

// Printh

/*
 * Helper to print headings
*/
static inline void print_header(const char *msg, const char *style) {
        fprintf(stderr, "%s%s%s", style, msg, COLOR_RESET);
}

/*
 * Helper qsort to sort commands
*/
static inline i32 qsort_compare_cmd(const void *a, const void *b) {
        const CLICommand *CMD_A = (const CLICommand *)a;
        const CLICommand *CMD_B = (const CLICommand *)b;

        char fullA[AOC_CLI_BUFFER], fullB[AOC_CLI_BUFFER];
        snprintf(fullA, sizeof(fullA), "%s", CMD_A->cmd);
        snprintf(fullB, sizeof(fullB), "%s", CMD_B->cmd);

        return strcmp(fullA, fullB);
}

static inline const char *qsort_get_opt(const CLIOption *flag) {
        if (flag->short_opt) return flag->short_opt;
        if (flag->long_opt) return flag->long_opt;
        return "";
}

/*
 * Helper qsort to sort options
*/
static inline i32 qsort_compare_opts(const void *a, const void *b) {
        const CLIOption *FLAG_A = (const CLIOption *)a;
        const CLIOption *FLAG_B = (const CLIOption *)b;

        return strcmp(qsort_get_opt(FLAG_A), qsort_get_opt(FLAG_B));
}

static inline bool has_commands(CLIProgram info) {
        for (usize i = 0; i < info.cmdc; i++) {
                if (info.commands->cmd != NULL) return true;
        }
        return false;
}

static inline bool has_args(CLIProgram info) {
        for (usize i = 0; i < info.cmdc; i++) {
                if (info.args->name != NULL) return true;
        }
        return false;
}

static inline bool has_options(CLIProgram info) {
        for (usize i = 0; i < info.flagc; i++) {
                if (info.flags->long_opt != NULL || info.flags->short_opt != NULL) return true;
        }
        return false;
}

// Arguments:
//   arg1
//       Description
//   arg2
//       Description
static inline void printh_arguments(CLIProgram *prog) {
        print_header("Arguments:\n", COLOR_BOLD_UNDERLINE);
        for (usize i = 0; i < prog->argc; i++) {
                const char *ARG_NAME = prog->args[i].name;
                const char *ARG_DESC = prog->args[i].desc;

                char arg_full[AOC_CLI_BUFFER] = { 0 };

                if (ARG_NAME) {
                        snprintf(arg_full, sizeof(arg_full), "%s%s%s", COLOR_BOLD, ARG_NAME,
                                 COLOR_RESET);
                        fprintf(stderr, "  %s\n", arg_full);
                        if (ARG_DESC && strlen(ARG_DESC) > 0) {
                                aoc_iprint(ARG_DESC, PRINTH_DESC_INDENT);
                                fputc('\n', stderr);
                        }
                }
        }
        fputc('\n', stderr);
}

// Commands:
//   cmd1 <ARG>
//       Description
//   cmd2 [ARG]
//       Description
static inline void printh_commands(CLIProgram *prog) {
        print_header("Commands:\n", COLOR_BOLD_UNDERLINE);

        for (usize i = 0; i < prog->cmdc; i++) {
                const char *CMD = prog->commands[i].cmd;
                const char *ARG = (prog->commands[i].args) ? prog->commands[i].args->name : NULL;
                const char *DESC = prog->commands[i].desc;

                char cmd_full[AOC_CLI_BUFFER] = { 0 };

                if (ARG) {
                        switch (prog->commands[i].args->type) {
                        case ReqArg:
                                snprintf(cmd_full, sizeof(cmd_full), "%s%s%s <%s>", COLOR_BOLD, CMD,
                                         COLOR_RESET, ARG);
                                break;
                        case NReqArg:
                                snprintf(cmd_full, sizeof(cmd_full), "%s%s%s [%s]", COLOR_BOLD, CMD,
                                         COLOR_RESET, ARG);
                                break;
                        }
                } else {
                        snprintf(cmd_full, sizeof(cmd_full), "%s%s%s", COLOR_BOLD, CMD,
                                 COLOR_RESET);
                }

                fprintf(stderr, "  %s\n", cmd_full);
                if (DESC && strlen(DESC) > 0) {
                        aoc_iprint(DESC, PRINTH_DESC_INDENT);
                        fputc('\n', stderr);
                }
        }
        fputc('\n', stderr);
}

// Options:
//   -s, --short [ARG]
//   -l, --long <ARG>
//   -h, --help
//       Description
static inline void printh_options(CLIProgram *prog) {
        print_header("Options:\n", COLOR_BOLD_UNDERLINE);

        for (usize i = 0; i < prog->flagc; i++) {
                const char *SHORT_OPT = prog->flags[i].short_opt;
                const char *LONG_OPT = prog->flags[i].long_opt;
                const char *ARG = (prog->flags[i].args) ? prog->flags[i].args->name : NULL;
                const char *DESC = prog->flags[i].desc;

                char flag_buffer[AOC_CLI_BUFFER] = { 0 };

                if (SHORT_OPT && LONG_OPT) {
                        snprintf(flag_buffer, sizeof(flag_buffer), "%s%s%s, %s%s%s", COLOR_BOLD,
                                 SHORT_OPT, COLOR_RESET, COLOR_BOLD, LONG_OPT, COLOR_RESET);
                } else if (LONG_OPT) {
                        snprintf(flag_buffer, sizeof(flag_buffer), "%s%s%s", COLOR_BOLD, LONG_OPT,
                                 COLOR_RESET);
                } else if (SHORT_OPT) {
                        snprintf(flag_buffer, sizeof(flag_buffer), "%s%s%s", COLOR_BOLD, SHORT_OPT,
                                 COLOR_RESET);
                }

                if (ARG) {
                        switch (prog->flags->args->type) {
                        case ReqArg:
                                strcat(flag_buffer, " <");
                                strcat(flag_buffer, ARG);
                                strcat(flag_buffer, ">");
                                break;
                        case NReqArg:
                                strcat(flag_buffer, " [");
                                strcat(flag_buffer, ARG);
                                strcat(flag_buffer, "]");
                                break;
                        }
                }

                fprintf(stderr, "  %s\n", flag_buffer);
                if (DESC && strlen(DESC) > 0) {
                        aoc_iprint(DESC, PRINTH_DESC_INDENT);
                        fputc('\n', stderr);
                }
        }
}

void _aoc_printh(CLIProgram prog) {
        qsort(prog.commands, prog.cmdc, sizeof(CLICommand), qsort_compare_cmd);
        qsort(prog.flags, prog.flagc, sizeof(CLIOption), qsort_compare_opts);

        // program name | program description
        fprintf(stderr, "%s | %s\n\n", prog.name, prog.desc);

        // Usage: program usage
        print_header("Usage:", COLOR_BOLD_UNDERLINE);
        fprintf(stderr, "  %s%s%s %s\n\n", COLOR_BOLD, prog.name, COLOR_RESET, prog.usage);

        if (has_args(prog)) printh_arguments(&prog);
        if (has_commands(prog)) printh_commands(&prog);
        if (has_options(prog)) printh_options(&prog);
}

// Zshgen

static inline void zshgen_print_arg_autocomplete(const CLIProgram *prog, const CLIArgument *args) {
        char ARG[AOC_CLI_BUFFER] = { 0 };
        if (args)
                cli_normalize_shell_name(ARG, args->name, AOC_CLI_BUFFER);
        else
                return;
        const char *COMPLETIONS = args->completion;
        if (COMPLETIONS != NULL) {
                printf("_%s_get_%s() {\n", prog->name, ARG);
                printf("  local results\n");
                printf("  results=(${(f)\"$(%s 2>/dev/null)\"})\n", COMPLETIONS);
                printf("  compadd -Q -a results\n");
                printf("}\n\n");
        }
}

static void zshgen_print_flag_arg(const CLIProgram *prog, const CLIOption *flag) {
        char ARG[AOC_CLI_BUFFER];
        const char *SHORT_FLAG = flag->short_opt;
        const char *LONG_FLAG = flag->long_opt;
        if (flag->args)
                cli_normalize_shell_name(ARG, flag->args->name, AOC_CLI_BUFFER);
        else
                return;
        const char *DESC = flag->desc;
        const char *COMP = (flag->args) ? flag->args->completion : NULL;

        if (!COMP) return;

        if (LONG_FLAG) {
                printf("    '%s", LONG_FLAG);
                if (DESC) printf("=[%s]", DESC);
                if (COMP) {
                        printf(":%s:_%s_get_%s", ARG, prog->name, ARG);
                } else {
                        printf(":%s", ARG);
                }
                printf("' \\\n");
        }
        if (SHORT_FLAG) {
                printf("    '%s", SHORT_FLAG);
                if (DESC) printf("[%s]", DESC);
                if (COMP) {
                        printf(":%s:_%s_get_%s", ARG, prog->name, ARG);
                } else {
                        printf(":%s", ARG);
                }
                printf("' \\\n");
        }
}

static inline void zshgen_print_command_case(const CLIProgram *prog, const CLICommand *cmd) {
        char ARG[AOC_CLI_BUFFER];
        if (cmd->args)
                cli_normalize_shell_name(ARG, cmd->args->name, AOC_CLI_BUFFER);
        else
                return;
        const char *COMP = (cmd->args) ? cmd->args->completion : NULL;
        if (!COMP) return;

        printf("        %s)\n", cmd->cmd);
        printf("          _arguments \\\n");
        printf("            '*:%s:_%s_get_%s' \\\n", ARG, prog->name, ARG);
        printf("          ;;\n");
}

static inline void zshgen_print_flag_case(const CLIProgram *prog, const CLIOption *flag) {
        char ARG[AOC_CLI_BUFFER];
        const char *SHORT_FLAG = flag->short_opt;
        const char *LONG_FLAG = flag->long_opt;
        if (flag->args)
                cli_normalize_shell_name(ARG, flag->args->name, AOC_CLI_BUFFER);
        else
                return;
        const char *COMPLETIONS = (flag->args) ? flag->args->completion : NULL;
        if (!COMPLETIONS) return;

        if (SHORT_FLAG != NULL && LONG_FLAG != NULL) {
                printf("        %s | %s)\n", SHORT_FLAG, LONG_FLAG);
        } else if (SHORT_FLAG != NULL) {
                printf("        %s)\n", SHORT_FLAG);
        } else if (LONG_FLAG != NULL) {
                printf("        %s)\n", LONG_FLAG);
        }
        printf("          _arguments \\\n");
        printf("            '*:%s:_%s_get_%s' \\\n", ARG, prog->name, ARG);
        printf("          return\n");
        printf("          ;;\n");
}

void _aoc_zshgen(const CLIProgram prog, const CLIEnv *env, usize envc) {
        // Header
        printf("#compdef %s\n\n", prog.name);

        // Environment defaults
        for (usize i = 0; i < envc; i++) {
                printf("%s=%s\n", env[i].name, env[i].value);
        }

        // Main function
        printf("_%s() {\n", prog.name);
        printf("  local -a subcommands\n\n");

        // Define Subcommands
        printf("  subcommands=(\n");
        for (usize i = 0; i < prog.cmdc; i++) {
                printf("    \"%s:%s\"\n", prog.commands[i].cmd, prog.commands[i].desc);
        }
        printf("  )\n\n");

        // Define arguments
        printf("  _arguments -C \\\n");
        printf("    '1:command:->subcmds' \\\n");
        for (usize i = 0; i < prog.flagc; i++) {
                zshgen_print_flag_arg(&prog, &prog.flags[i]);
        }
        printf("    '*::args:->command_args'\n\n");

        // Autocompletion
        printf("  case $state in\n");
        printf("    subcmds)\n");
        printf("      _describe 'command' subcommands\n");
        printf("      return\n");
        printf("      ;;\n");
        printf("    command_args)\n");
        printf("      case $words[1] in\n");

        // Autocomplete arguments from commands
        for (usize i = 0; i < prog.cmdc; i++) {
                zshgen_print_command_case(&prog, &prog.commands[i]);
        }
        // Autocomplete arguments from flags
        for (usize i = 0; i < prog.flagc; i++) {
                zshgen_print_flag_case(&prog, &prog.flags[i]);
        }
        printf("      esac\n");
        printf("      ;;\n");
        printf("  esac\n");
        printf("}\n\n");

        // Define helper functions to autocomplete arguments
        for (usize i = 0; i < prog.argc; i++) {
                zshgen_print_arg_autocomplete(&prog, &prog.args[i]);
        }

        // Assign function to program
        printf("compdef _%s %s\n", prog.name, prog.name);
}
#endif

#ifdef DEBUG_HEAP
typedef struct debug_entry {
        void *ptr;
        size_t size;
        const char *func;
        const char *file;
        int line;
        struct debug_entry *next;
} debug_entry;

/*
 * Replaces allocators with custom debug ones
*/
static void *(*std_malloc)(size_t) = NULL;
static void *(*std_calloc)(size_t, size_t) = NULL;
static void *(*std_realloc)(void *, size_t) = NULL;
static void (*std_free)(void *) = NULL;

static void init_std_functions(void) {
        if (std_malloc) return;
        std_malloc = malloc;
        std_calloc = calloc;
        std_realloc = realloc;
        std_free = free;
}

static debug_entry *head = NULL;
static int debug_alloc_count = 0;
static int debug_free_count = 0;

static void add_entry(void *ptr, size_t size, const char *func, const char *file, int line) {
        init_std_functions();
        debug_entry *entry = (debug_entry *)std_malloc(sizeof(debug_entry));
        if (!entry) return;

        entry->ptr = ptr;
        entry->size = size;
        entry->func = func;
        entry->file = file;
        entry->line = line;
        entry->next = head;
        head = entry;
        debug_alloc_count++;
}

static void remove_entry(void *ptr) {
        init_std_functions();
        debug_entry **curr = &head;
        while (*curr) {
                if ((*curr)->ptr == ptr) {
                        debug_entry *to_free = *curr;
                        *curr = to_free->next;
                        std_free(to_free);
                        debug_free_count++;
                        return;
                }
                curr = &(*curr)->next;
        }
}

void *debug_malloc(size_t size, const char *func, const char *file, int line) {
        init_std_functions();
        void *ptr = std_malloc(size);
        if (ptr) {
                add_entry(ptr, size, func, file, line);
        }
        return ptr;
}

void *debug_calloc(size_t nmemb, size_t size, const char *func, const char *file, int line) {
        init_std_functions();
        void *ptr = std_calloc(nmemb, size);
        if (ptr) {
                add_entry(ptr, nmemb * size, func, file, line);
        }
        return ptr;
}

void *debug_realloc(void *ptr, size_t size, const char *func, const char *file, int line) {
        init_std_functions();
        if (ptr == NULL) {
                void *new_ptr = std_malloc(size);
                if (new_ptr) add_entry(new_ptr, size, func, file, line);
                return new_ptr;
        }

        debug_entry **entry_ptr = &head;
        while (*entry_ptr && (*entry_ptr)->ptr != ptr) {
                entry_ptr = &(*entry_ptr)->next;
        }

        void *new_ptr = std_realloc(ptr, size);
        if (new_ptr && *entry_ptr) {
                (*entry_ptr)->ptr = new_ptr;
                (*entry_ptr)->size = size;
                (*entry_ptr)->func = func;
                (*entry_ptr)->file = file;
                (*entry_ptr)->line = line;
        } else if (new_ptr) {
                add_entry(new_ptr, size, func, file, line);
        }
        return new_ptr;
}

void debug_free(void *ptr, const char *func, const char *file, int line) {
        if (!ptr) return;
        remove_entry(ptr);
        init_std_functions();
        std_free(ptr);
}

int debug_count_leaks(void) {
        return debug_alloc_count - debug_free_count;
}

void debug_memory_summary(FILE *ref fd) {
        debug_entry *curr = head;
        int leaks_found = 0;
        fprintf(fd, "===== Memory Summary Report =====\n");
        fprintf(fd, "Total allocations: %d\n", debug_alloc_count);
        fprintf(fd, "Total frees: %d\n", debug_free_count);
        fprintf(fd, "Active allocations (leaks): %d\n", debug_alloc_count - debug_free_count);
        while (curr) {
                fprintf(fd, "%zu bytes at %s() in %s:%d (ptr: %p)\n", curr->size, curr->func,
                        curr->file, curr->line, curr->ptr);
                leaks_found += curr->size;
                curr = curr->next;
        }
        if (debug_alloc_count == debug_free_count) {
                fprintf(fd, "No memory leaks detected.\n");
        }
}

#define malloc(x) debug_malloc(x, __func__, __FILE__, __LINE__)
#define calloc(n, x) debug_calloc(n, x, __func__, __FILE__, __LINE__)
#define realloc(p, x) debug_realloc(p, x, __func__, __FILE__, __LINE__)
#define free(x) debug_free(x, __func__, __FILE__, __LINE__)
#endif /* DEBUG_HEAP */

#ifdef AOCLIBS_ALLOC
void Free(void *ref ptr) {
        ASSERT(ptr != NULL, "double free attempt");
        free(ptr);
}

void *xnull Calloc(usize count, usize size) {
        void *tmp = calloc(count, size);
        if (!tmp) {
                PTRACE("[calloc] %s [Requested: %zu bytes] (%s:%s)\n", strerror(errno),
                       count * size);
                return NULL;
        }
        return tmp;
}

void *xnull Malloc(usize cap) {
        void *tmp = malloc(cap);
        if (!tmp) {
                PTRACE("[calloc] %s [Requested: %zu bytes]\n", strerror(errno), cap);
                return NULL;
        }
        return tmp;
}

void *null Realloc(void *null ptr, usize cap) {
        void *tmp = realloc(ptr, cap);
        if (!tmp) {
                PTRACE("[realloc] %s [Requested: %zu bytes]\n", strerror(errno), cap);
                return NULL;
        }
        return tmp;
}
#endif

#ifdef AOCLIBS_STRING
#define StrError ((size_t)-1)

typedef enum {
        StringHeap,
        StringStack,
        StringLiteral,
} StringType;

#define get_slice(s) (s.len), (s.slice)
typedef struct {
        const char *slice;
        int len;
} StrSlice;

typedef struct {
        char *str;
        size_t len;
        size_t cap;
        StringType type;
} str;

#define str_to_slice(s, x, y) cstr_to_slice((s.str), (x), (y))
StrSlice cstr_to_slice(const char *ref s, size_t start, size_t end) {
        ASSERT_REF(s != NULL);
        return (StrSlice){ .slice = s + start, .len = end - start };
}

bool str_can_mut(const str *null s) {
        if (!s) return false;
        if (s->type == StringLiteral) return false;
        return true;
}

bool str_is_null(const str *null s) {
        return !s || !s->str || s->len == 0;
}

bool str_is_null_assert(const str *null s) {
        return !(!s || !s->str);
}

#define cstr_dup(s, len) _cstr_dup((s), (len + 1))
char *xnull _cstr_dup(const char *ref s, const size_t len) {
        ASSERT_REF(s != NULL);
        char *d = malloc(len);
        if (!d) return NULL;
        return memcpy(d, s, len);
}

str str_dup(const str *ref s) {
        ASSERT_REF(str_is_null_assert(s));
        ASSERT(s->type == StringHeap, "string is not heap allocated");
        return (str){
                .str = _cstr_dup(s->str, s->cap),
                .len = s->len,
                .cap = s->cap,
                .type = StringHeap,
        };
}

void cstr_to_lower(char *ref s, const size_t len) {
        ASSERT_REF(s != NULL);
        for (size_t i = 0; i < len; i++) {
                s[i] = tolower(s[i]);
        }
}

#define str_eq(s1, s2) cstr_eq((s1.str), (s2.str))
#define str_eq_case(s1, s2) cstr_eq_case((s1.str), (s2.str))

bool cstr_eq(const char *xref s1, const char *xref s2) {
        if (!s1 || !s2) return s1 == s2;
        const unsigned char *s1_tmp = (void *)s1;
        const unsigned char *s2_tmp = (void *)s2;
        while (*s1_tmp && *s2_tmp && *s1_tmp == *s2_tmp) {
                s1_tmp++;
                s2_tmp++;
        }
        return *s1_tmp == *s2_tmp;
}

bool cstr_eq_case(const char *xref s1, const char *xref s2) {
        if (!s1 || !s2) return s1 == s2;
        const unsigned char *s1_tmp = (void *)s1;
        const unsigned char *s2_tmp = (void *)s2;
        while (*s1_tmp && *s2_tmp && tolower(*s1_tmp) == tolower(*s2_tmp)) {
                s1_tmp++;
                s2_tmp++;
        }
        return tolower(*s1_tmp) == tolower(*s2_tmp);
}

#define str_len_comptime(s) ((sizeof((" " s " ")) / sizeof((s)[0])) - sizeof((s)[0]))

size_t str_len(const char *null s, const size_t buff) {
        if (s == NULL) return 0;
        const char *s_tmp = memchr(s, 0, buff);
        return s_tmp ? s_tmp - s : buff;
}

#define str_new_stack(cap) _str_new_stack(alloca((cap)), cap)
str _str_new_stack(char *ref s, const size_t cap) {
        ASSERT_REF(s != NULL);
        return (str){ .str = s, .len = 0, .cap = cap, .type = StringStack };
}

#define str_new_comptime(s) _str_new_comptime((" " s " "), str_len_comptime(s))
str _str_new_comptime(char *ref s, const size_t len) {
        ASSERT_REF(s != NULL);
        return (str){ .str = s, .len = 0, .cap = len, .type = StringLiteral };
}

#define str_new_heap(s, cap) _str_new_heap((s), cap)
str _str_new_heap(char *null s, const size_t cap) {
        return (str){
                .str = s,
                .len = 0,
                .cap = cap,
                .type = StringHeap,
        };
}

int str_resize(str *ref s, size_t cap) {
        ASSERT_REF(str_is_null_assert(s));
        ASSERT(s->type == StringHeap, "string is not heap allocated");
        void *tmp = realloc(s->str, cap);
        if (tmp == NULL) return -1;
        s->str = tmp;
        return 0;
}

void str_free(str *ref s) {
        ASSERT(str_is_null_assert(s), "double free attempt");
        ASSERT(s->type == StringHeap, "string is not heap allocated");
        free(s->str);
        s->len = 0;
        s->cap = 0;
        s = NULL;
}

void str_free_array(str *ref s[], size_t len) {
        for (size_t i = 0; i < len; i++) {
                str_free(s[i]);
        }
}

void str_erase(str *ref s) {
        ASSERT(str_is_null_assert(s), "double free attempt");
        ASSERT(s->type != StringLiteral, "attempt to modify string literal");
        for (size_t i = 0; i < s->cap; i++) {
                s->str[i] = '\0';
        }
        if (s->type == StringHeap) str_free(s);
}

void str_clear(str *ref s) {
        ASSERT_REF(str_is_null_assert(s));
        ASSERT(s->type != StringLiteral, "attempt to modify string literal");
        s->str[0] = '\0';
        s->len = 0;
}

#define cstr_begins_with(s, begin, begin_len, s_len) \
        cstr_match_pos((s), (begin), (begin_len), (s_len), 0)
#define cstr_ends_with(s, end, end_len, s_len) \
        cstr_match_pos((s), (end), (end_len), (s_len), (s_len) - (end_len))
bool cstr_match_pos(const char *xnull s, const char *xnull pattern, size_t pattern_len,
                    size_t s_len, size_t offset) {
        if (!s || pattern_len > s_len || offset > s_len - pattern_len) return false;

        size_t i = offset + pattern_len;
        size_t j = pattern_len;

        while (j > 0) {
                i--;
                j--;
                if (s[i] != pattern[j]) return false;
        }
        return true;
}

#define str_overwrite(s1, s2) str_copy((s1), (s2), 0)
#define str_cat(s1, s2) str_copy((s1), (s2), s1.len)
#define str_append(s1, s2) str_copy((s1), (s2), (s1.len + 1))
int str_copy(str *xref s1, const str *xref s2, const size_t s1_offset) {
        ASSERT_REF(str_is_null_assert(s1));
        ASSERT_REF(str_is_null_assert(s2));
        ASSERT(s1->type != StringLiteral, "attempt to modify string literal");
        if (!str_can_mut(s1) || str_is_null_assert(s2)) return -1;

        size_t avail = s1->cap - s1_offset;
        size_t needed = s2->len + 1;

        if (needed > avail) {
                if (s1->type != StringHeap) return -1;

                if (str_resize(s1, s1_offset + needed) != 0) return -1;
        }

        memcpy(s1->str + s1_offset, s2->str, s2->len);
        s1->str[s1_offset + s2->len] = '\0';

        s1->len = s1_offset + s2->len;

        return 0;
}

int str_push(str *ref s, char c) {
        ASSERT_REF(str_is_null_assert(s));
        ASSERT(s->type != StringLiteral, "attempt to modify string literal");

        usize needed = s->len + 2;
        if (needed > s->cap) {
                if (str_resize(s, needed) != 0) return -1;
        }

        ((char *)s->str)[s->len] = c;
        ((char *)s->str)[s->len + 1] = '\0';
        s->len++;
        return 0;
}

int str_pop(str *ref s) {
        ASSERT_REF(str_is_null_assert(s));
        ASSERT(s->type != StringLiteral, "attempt to modify string literal");

        if (s->len == 0) return -1;

        s->len--;
        ((char *)s->str)[s->len] = '\0';
        return 0;
}

int str_drop(str *ref s, usize index) {
        ASSERT_REF(str_is_null_assert(s));
        ASSERT(s->type != StringLiteral, "attempt to modify string literal");
        if (index >= s->len) return -1;

        char *ptr = (char *)s->str;

        memmove(&ptr[index], &ptr[index + 1], s->len - index);

        s->len--;
        ptr[s->len] = '\0';

        return 0;
}

#define cster_overwrite(s1, s2, s2_len) cstr_copy((s1), (s2), 0, s2_len)
#define cster_overwrite_comptime(s1, s2) cstr_copy((s1), (s2), 0, str_len_comptime(s2))
#define cstr_cat(s1, s2) cstr_copy((s1), (s2), s1.len, str_len((s2)))
#define cstr_cat_comptime(s1, s2) cstr_copy((s1), (s2), s1.len, str_len_comptime((s2)))
#define cstr_append(s1, s2) cstr_copy((s1), (s2), (s1.len + 1), str_len((s2)))
#define cstr_append_comptime(s1, s2) cstr_copy((s1), (s2), (s1.len + 1), str_len_comptime((s2)))
#define str_null_terminate(s) cstr_copy((s), "\0", (*s.len), 1)
int cstr_copy(str *xref s1, const char *xref s2, const size_t s1_offset, size_t s2_len) {
        ASSERT_REF(str_is_null_assert(s1));
        ASSERT_REF(s2 != NULL);
        ASSERT(s1->type != StringLiteral, "attempt to modify string literal");
        if (!str_can_mut(s1) || !s2) return -1;

        size_t avail = s1->cap - s1_offset;
        size_t needed = s2_len + 1;

        if (needed > avail) {
                if (s1->type != StringHeap) return -1;
                if (str_resize(s1, s1_offset + needed) != 0) return -1;
        }

        memcpy(s1->str + s1_offset, s2, s2_len);
        s1->str[s1_offset + s2_len] = '\0';

        s1->len = s1_offset + s2_len;

        return 0;
}

int cstr_copy_fmt(str *xref s, const char *xref fmt, ...) {
        ASSERT_REF(str_is_null_assert(s));
        ASSERT_REF(fmt != NULL);
        ASSERT(s->type != StringLiteral, "attempt to modify string literal");
        if (!str_can_mut(s) || !fmt) return -1;

        int needed_len = 0, allocated_len = 0;

        va_list args;
        va_start(args, fmt);
        needed_len = vsnprintf(s->str, 0, fmt, args);
        va_end(args);

        if (needed_len < 0) return -1;

        if ((size_t)needed_len >= s->cap) {
                if (s->type == StringHeap)
                        if (str_resize(s, needed_len + 1) != 0) return -1;
                va_start(args, fmt);
                allocated_len = vsnprintf(s->str, s->cap, fmt, args);
                va_end(args);
                if (allocated_len < 0) return -1;
        }

        s->len = allocated_len;
        return 0;
}

bool match_delim_rec(const char *xnull s, size_t s_len, const char *xnull delim, size_t delim_len,
                     size_t pos, size_t j) {
        if (!s || !delim) return false;
        if (j == delim_len) return true;
        if (pos + j >= s_len) return false;
        if (s[pos + j] != delim[j]) return false;
        return match_delim_rec(s, s_len, delim, delim_len, pos, j + 1);
}

size_t str_chr_str(const str *xref s, const char *xref delim, const size_t delim_len) {
        ASSERT_REF(str_is_null_assert(s));
        ASSERT_REF(delim != NULL);

        for (size_t i = 0; i < s->len; i++) {
                if (match_delim_rec(s->str, s->len, delim, delim_len, i, 0)) {
                        return (size_t)i;
                }
        }

        return StrError;
}

size_t str_chr(const str *ref s, char delim) {
        ASSERT_REF(str_is_null_assert(s));

        for (size_t i = 0; i < s->len; i++) {
                if (s->str[i] == delim) return i;
        }

        return StrError;
}

const char *null str_tok_str_const(const str *xref s, const char *xref delim,
                                   const size_t delim_len) {
        ASSERT_REF(str_is_null_assert(s));
        ASSERT_REF(delim != NULL);

        size_t pos = str_chr_str(s, delim, delim_len);
        if (pos == StrError) return NULL;

        return s->str + pos + delim_len;
}

const char *null str_tok(const str *ref s, char delim) {
        ASSERT_REF(str_is_null_assert(s));

        size_t pos = str_chr(s, delim);
        if (pos == StrError) return NULL;

        return s->str + pos + 1;
}

size_t cstr_trim_whitespace(char *ref s, const size_t len) {
        ASSERT_REF(s != NULL);

        if (len == 0) return 0;

        usize i = len;
        while (i > 0 && isspace((unsigned char)s[i - 1])) {
                i--;
        }
        s[i] = '\0';
        return len;
}

void str_trim_whitespace(str *ref s) {
        ASSERT_REF(str_is_null_assert(s));
        ASSERT(s->type != StringLiteral, "attempt to modify string literal");

        size_t len = cstr_trim_whitespace(s->str, s->len);
        s->len = len;
}

double cstr_to_double(const char *ref s, const double _default) {
        ASSERT_REF(s != NULL);
        char *endptr;
        double val = strtod(s, &endptr);
        if (*endptr != '\0') {
                return _default;
        }
        return val;
}

bool cstr_to_bool(const char *ref s, const bool _default) {
        ASSERT_REF(s != NULL);
        if (cstr_eq_case(s, "true") || cstr_eq(s, "1")) return true;
        if (cstr_eq_case(s, "false") || cstr_eq(s, "0")) return false;
        return _default;
}

float cstr_to_float(const char *ref s, const float _default) {
        ASSERT_REF(s != NULL);
        char *endptr;
        float val = strtof(s, &endptr);
        if (*endptr != '\0') {
                return _default;
        }
        return val;
}

long cstr_to_long(const char *ref s, const long _default) {
        ASSERT_REF(s != NULL);
        char *endptr;
        long val = strtol(s, &endptr, 10);
        if (*endptr != '\0') {
                return _default;
        }
        return val;
}
#endif

#endif // AOCLIBS_H
#endif // AOCLIBS_IMPLEMENTATION
