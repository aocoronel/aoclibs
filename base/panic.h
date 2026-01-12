#ifndef AOCLIBS_PANIC_H_
#define AOCLIBS_PANIC_H_

#include <stdio.h>
#include <stdlib.h>

/*
 * Used to panic, when an unreachable code is ran
*/
#include "attributes.h"
#define _unreachable aoc_panic(__FILE__, __LINE__, __func__, "unreachable code")

/*
 * Prints message, including metadata and aborts the program
*/
#define panic(msg) aoc_panic(__FILE__, __LINE__, __func__, msg)

/*
 * Prints message, including metadata and aborts the program
 *
 * Should not be used directly. Use the panic macro, instead.
*/
AOCLIBS_PREFIX void aoc_panic(const char *__file, int __line, const char *__func, const char *msg) __attribute__((noreturn));

AOCLIBS_PREFIX void aoc_panic(const char *__file, int __line, const char *__func, const char *msg) {
        fprintf(stderr, "PANIC: %s at %s:%d (%s) ", msg, __file, __line, __func);
        fflush(stderr);
        abort();
}

#endif // AOCLIBS_PANIC_H_
