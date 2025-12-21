#include <aoclibs/common.h>
#include <stdlib.h>
#include <stdio.h>

void _panic(const char *__file, int __line, const char *__func,
                      const char *msg) FN_ABORTS {
        fprintf(stderr, "PANIC: %s at %s:%d (%s) ", msg, __file, __line, __func);
        fflush(stderr);
        abort();
}
