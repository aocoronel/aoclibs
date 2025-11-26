#ifndef AOCLIBS_ERROR_MACROS_H
#define AOCLIBS_ERROR_MACROS_H

#include <a_error_types.h>

/*
 * try allows to return earlier when the calling and caller functions return
 * the Error type
*/
#define try(e) do { Error _err = (e); if (_err.code != 0) return _err; } while(0)
#define try_cleanup(e, cleanup) do { \
    Error _err = (e); \
    if (_err.code != 0) { \
        cleanup; \
        return _err; \
    } \
} while(0)

/*
 * try_main allows to return earlier in the main function, if the calling function
 * returns the Error type. try_main returns the Error error code
*/
#define try_main(e) do { Error _err = (e); if (_err.code != 0) return _err.code; } while(0)
#define try_main_cleanup(e, cleanup) do { \
    Error _err = (e); \
    if (_err.code != 0) { \
        cleanup; \
        return _err.code; \
    } \
} while(0)

#endif
