#include <a_error_fn_err.h>
#include <stddef.h>

Error err(i32 code, const char msg[]) {
        if (msg == NULL) {
                return (Error){ code, "undefined" };
        }
        return (Error){ code, msg };
}
