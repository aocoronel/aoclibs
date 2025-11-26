#ifndef AOCLIBS_ERROR_TYPES_H
#define AOCLIBS_ERROR_TYPES_H

#include <a_types.h>

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

#endif
