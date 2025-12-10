#include <aoclibs/error.h>
#include <stdio.h>

Err rerr(Err err, FILE *output) {
        if (fprintf(output, "%s", err.msg) < 0) {
                return werr(-1, "failed to print error message");
        } else {
                return ok();
        }
}

Err rerrln(Err err, FILE *output) {
        if (fprintf(output, "%s\n", err.msg) < 0) {
                return werr(-1, "failed to print error message");
        } else {
                return ok();
        }
}
