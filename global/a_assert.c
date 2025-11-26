#include <a_error.h>
#include <stdio.h>
#include <stdlib.h>

void _assert_ok(Error e, const char *file, int line, const char *func) {
    if (e.code != 0) {
        fprintf(stderr, "Assertion failed: %s\n  at %s:%d (%s)\n",
                e.msg, file, line, func);
        abort();
    }
}

void _assert_err(Error e, const char *file, int line, const char *func) {
    if (e.code == 0) {
        fprintf(stderr, "Assertion failed: %s\n  at %s:%d (%s)\n",
                e.msg, file, line, func);
        abort();
    }
}
