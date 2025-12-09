#include <aoclibs/error.h>
#include <stddef.h>

Err err(i32 code, const char msg[]) {
        if (msg == NULL) return (Err){ code, "undefined" };
        return (Err){ code, msg };
}
