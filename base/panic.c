#include "panic.h"
#include <stdio.h>
#include <stdlib.h>

void aoc_panic(const char *__file, int __line, const char *__func, const char *msg) {
        fprintf(stderr, "PANIC: %s at %s:%d (%s) ", msg, __file, __line, __func);
        fflush(stderr);
        abort();
}
