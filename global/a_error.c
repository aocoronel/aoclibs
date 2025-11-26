#include <a_error.h>
#include <stdio.h>

Error err(i32 code, const char msg[]) {
        if (msg == NULL) {
                return (Error){ code, "undefined" };
        }
        return (Error){ code, msg };
}

Error ok(void) {
        return (Error){ 0, "OK" };
}

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
