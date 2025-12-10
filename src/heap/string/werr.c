#include <aoclibs/common.h>
#include <aoclibs/heap/string.h>

eString werr_string(Err err, String s) {
        return (eString){ .err = err, .value = s };
}
