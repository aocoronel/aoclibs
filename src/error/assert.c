#include <aoclibs/assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void _assert(const char *expr, const char *file, unsigned line,
             const char *func, const char *fmt, ...) {
        va_list args;
        va_start(args, fmt);
        fprintf(stderr, "Assertion failed: ");
        vfprintf(stderr, fmt, args);
        fprintf(stderr, "\n%s at %s:%u (%s)\n", expr, file, line, func);
        va_end(args);

        abort();
}
