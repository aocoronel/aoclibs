#include <a_error_types.h>
#include <stdio.h>
#include <stdlib.h>

void _assert_ok(Error e, const char *file, int line, const char *func) {
    if (e.code != 0) {
        fprintf(stderr, "Assertion failed. Expected success: %s\n  at %s:%d (%s)\n",
                e.msg, file, line, func);
        abort();
    }
}

void _assert_err(Error e, const char *file, int line, const char *func) {
    if (e.code == 0) {
        fprintf(stderr, "Assertion failed. Expected error: %s\n  at %s:%d (%s)\n",
                e.msg, file, line, func);
        abort();
    }
}
