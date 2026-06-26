#ifndef AOC_DEBUG_H_
#define AOC_DEBUG_H_

#ifdef AOC_DEBUG

#define _GNU_SOURCE
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <execinfo.h>
#include <unistd.h>

static jmp_buf __DEBUG_JMP;

static void __debug_backtrace(void) {
	void *array[128];
	size_t size = backtrace(array, 128);
	char **strings = backtrace_symbols(array, size);
	for (size_t i = 0; i < size; i++) {
		fprintf(stderr, "%s\n", strings[i]);
	}
	free(strings);
}

static void __debug_signal_handler(int sig) {
	fprintf(stderr, "[FATAL] Caught signal %d (%s)\n", sig, strsignal(sig));
	__debug_backtrace();
	longjmp(__DEBUG_JMP, 1);
}

static void __debug_warning_handler(int sig) {
	fprintf(stderr, "[WARNING] Caught signal %d (%s)\n", sig, strsignal(sig));
	__debug_backtrace();
	longjmp(__DEBUG_JMP, 1);
}

int main(int argc, char *argv[]);
__attribute__((constructor)) void _main(int argc, char *argv[]) {
	if (setjmp(__DEBUG_JMP) == 0) {
		signal(SIGSEGV, __debug_signal_handler);
		signal(SIGINT, __debug_warning_handler);
		signal(SIGABRT, __debug_signal_handler);
		signal(SIGFPE, __debug_signal_handler);
		signal(SIGILL, __debug_signal_handler);
		signal(SIGTERM, __debug_signal_handler);
		signal(SIGBUS, __debug_signal_handler);
		signal(SIGQUIT, __debug_signal_handler);
		signal(SIGTSTP, __debug_warning_handler);

		main(argc, argv);
		exit(EXIT_SUCCESS);
	} else {
		exit(EXIT_FAILURE);
	}
}

#endif // AOC_DEBUG

#endif // AOC_DEBUG_H_
