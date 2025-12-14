#include <aoclibs/error.h>
#include <stdio.h>

Err read_err(Err err, FILE *output) {
        if (fprintf(output, "%s", err.msg) < 0) {
                return werr(-1, "failed to print error message");
        } else {
                return ok();
        }
}

Err read_errln(Err err, FILE *output) {
        if (fprintf(output, "%s\n", err.msg) < 0) {
                return werr(-1, "failed to print error message");
        } else {
                return ok();
        }
}
