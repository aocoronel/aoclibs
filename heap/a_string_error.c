#include <a_string_types.h>
#include <a_string_fn_error.h>

stringerr err_stringerr(Error err, String s) {
    return (stringerr){ .err = err, .value = s };
}
