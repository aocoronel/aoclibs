#ifndef AOCLIBS_STRING_MACROS_H
#define AOCLIBS_STRING_MACROS_H

#include <a_string_types.h>

#define try_stringerr(e, s)                                              \
    do {                                                                 \
        Error _err = (e);                                                \
        if (_err.code != 0)                                              \
            return (stringerr){ .err = _err, .value = (s) };             \
    } while (0)

#endif // AOCLIBS_STRING_MACROS_H
