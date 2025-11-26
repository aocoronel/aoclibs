#include <a_error_fn_panic.h>
#include <stdlib.h>
#include <stdio.h>

_Noreturn void _panic(const char *file, int line, const char *func,
                      const char *msg) {
        fprintf(stderr, "PANIC: %s at %s:%d (%s) ", msg, file, line, func);
        fflush(stderr);
        abort();
}
