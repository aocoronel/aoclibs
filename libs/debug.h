#ifndef AOCLIBS_DEBUG_H_
#define AOCLIBS_DEBUG_H_

#ifdef AOCLIBS_DEBUG

#define _GNU_SOURCE
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <execinfo.h>
#include <unistd.h>

static jmp_buf __AOC_DEBUG_JMP;

static void __aoc_debug_backtrace(void) {
    void *array[128];
    size_t size = backtrace(array, 128);
    char **strings = backtrace_symbols(array, size);
    for (size_t i = 0; i < size; i++) {
        fprintf(stderr, "%s\n", strings[i]);
    }
    free(strings);
}

static void __aoc_debug_signal_handler(int sig) {
    fprintf(stderr, "[FATAL] Caught signal %d (%s)\n", sig, strsignal(sig));
    __aoc_debug_backtrace();
    longjmp(__AOC_DEBUG_JMP, 1);
}

static void __aoc_debug_warning_handler(int sig) {
    fprintf(stderr, "[WARNING] Caught signal %d (%s)\n", sig, strsignal(sig));
    __aoc_debug_backtrace();
    longjmp(__AOC_DEBUG_JMP, 1);
}

int main(int argc, char *argv[]);
__attribute__((constructor)) void _main(int argc, char *argv[]) {
    if (setjmp(__AOC_DEBUG_JMP) == 0) {
        signal(SIGSEGV, __aoc_debug_signal_handler);
        signal(SIGINT, __aoc_debug_warning_handler);
        signal(SIGABRT, __aoc_debug_signal_handler);
        signal(SIGFPE, __aoc_debug_signal_handler);
        signal(SIGILL, __aoc_debug_signal_handler);
        signal(SIGTERM, __aoc_debug_signal_handler);
        signal(SIGBUS, __aoc_debug_signal_handler);
        signal(SIGQUIT, __aoc_debug_signal_handler);
        signal(SIGTSTP, __aoc_debug_warning_handler);

        main(argc, argv);
        exit(EXIT_SUCCESS);
    } else {
        exit(EXIT_FAILURE);
    }
}

#endif // AOCLIBS_DEBUG

#endif // AOCLIBS_DEBUG_H_
