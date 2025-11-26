#include <a_error_fn_print.h>
#include <a_error_fn_ok.h>
#include <stdio.h>

Error print_error(Error err, FILE *output) {
        if (fprintf(output, "%s", err.msg) < 0) {
                return (Error){ -1, (const char *)"Failed to write" };
        } else {
                return ok();
        }
}

Error println_error(Error err, FILE *output) {
        if (fprintf(output, "%s\n", err.msg) < 0) {
                return (Error){ -1, (const char *)"Failed to write" };
        } else {
                return ok();
        }
}
