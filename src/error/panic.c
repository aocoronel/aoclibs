#include <aoclibs/error.h>
#include <stdlib.h>
#include <stdio.h>

_Noreturn void _panic(const char *__file, int __line, const char *__func,
                      const char *msg) {
        fprintf(stderr, "PANIC: %s at %s:%d (%s) ", msg, file, line, func);
        fflush(stderr);
        abort();
}
