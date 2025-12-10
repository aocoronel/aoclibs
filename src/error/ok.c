#include <aoclibs/error.h>

Err ok(void) {
        return (Err){ Ok, "OK" };
}
