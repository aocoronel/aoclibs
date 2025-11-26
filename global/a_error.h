#ifndef AOCLIBS_ERROR_H
#define AOCLIBS_ERROR_H

#include <a_types.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Stores the error code and its message
*/
typedef struct {
        int code;
        const char *msg;
} Error;

typedef struct { Error err; char value; }  charerr;
typedef struct { Error err; char* value; } cstrerr;
typedef struct { Error err; double value; } doubleerr;
typedef struct { Error err; float value; } floaterr;
typedef struct { Error err; int value; } interr;
typedef struct { Error err; long value; }  longerr;
typedef struct { Error err; long long value; }  longlongerr;
typedef struct { Error err; short value; } shorterr;
typedef struct { Error err; void* value; } voiderr;

#ifdef __STDBOOL_H
typedef struct { Error err; bool value; } boolerr;
#endif /* __STDBOOL_H */

/*
 * The user can choose, whether to use AOCLIBS aliases or the original stdint
 * types:
*/

#ifdef AOCLIBS_TYPES_H
typedef struct { Error err; i8 value; } i8err;
typedef struct { Error err; i16 value; } i16err;
typedef struct { Error err; i32 value; } i32err;
typedef struct { Error err; i64 value; } i64err;

typedef struct { Error err; u8 value; } u8err;
typedef struct { Error err; u16 value; } u16err;
typedef struct { Error err; u32 value; } u32err;
typedef struct { Error err; u64 value; } u64err;

typedef struct { Error err; f32  value; } f32err;
typedef struct { Error err; f64 value; } f64err;
#elif defined(_STDINT_H)
typedef struct { Error err; int8_t  value; } i8err;
typedef struct { Error err; int16_t value; } i16err;
typedef struct { Error err; int32_t value; } i32err;
typedef struct { Error err; int64_t value; } i64err;

typedef struct { Error err; uint8_t value; } u8err;
typedef struct { Error err; uint16_t value; } u16err;
typedef struct { Error err; uint32_t value; } u32err;
typedef struct { Error err; uint64_t value; } u64err;

typedef struct { Error err; float  value; } f32err;
typedef struct { Error err; double value; } f64err;
#endif /* AOCLIBS_TYPES_H */

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

/*
 * Used to panic, when an unreachable code runs
*/
#define unreachable() panic(__FILE__, __LINE__, __func__, "unreachable code")

_Noreturn void panic(const char *file, int line, const char *func, const char *msg) {
    fprintf(stderr, "PANIC: %s at %s:%d (%s) ", msg, file, line, func);
    fflush(stderr);
    abort();
}

/*
 * Creates an Error type
*/
Error err(int code, const char msg[]);

/*
 * Returns success Error
 *
 * code: 0
 *
 * msg: "OK"
*/
Error ok(void);

/*
 * Prints error message
*/
Error print_error(Error error, FILE *output);

/*
 * Prints error message, and appends a new line
*/
Error println_error(Error err, FILE *output);

#endif
