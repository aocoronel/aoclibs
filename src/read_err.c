#include <aoclibs/common.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

Err read_err(Err err, FILE *_Nonnull fd) {
        ASSERT(fd != NULL, "%s", "passing NULL pointer to Nonnull parameter");
        if (fprintf(fd, "%s", err.msg) != 0) {
                return ErrErrno;
        } else {
                return ok();
        }
}

Err read_errln(Err err, FILE *_Nonnull fd) {
        ASSERT(fd != NULL, "%s", "passing NULL pointer to Nonnull parameter");
        if (fprintf(fd, "%s\n", err.msg) != 0) {
                return ErrErrno;
        } else {
                return ok();
        }
}
