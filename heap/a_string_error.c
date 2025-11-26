#include "a_string_types.h"

stringerr err_stringerr(Error err, String s) {
    return (stringerr){ .err = err, .value = s };
}
