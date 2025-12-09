#include <aoclibs/error.h>
#include <stdio.h>

Err err_print(Err err, FILE *output) {
        if (fprintf(output, "%s", err.msg) < 0) {
                return (Err){ -1, (const char *)"Failed to write" };
        } else {
                return ok();
        }
}

Err err_println(Err err, FILE *output) {
        if (fprintf(output, "%s\n", err.msg) < 0) {
                return (Err){ -1, (const char *)"Failed to write" };
        } else {
                return ok();
        }
}
