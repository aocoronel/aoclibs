#ifndef AOCLIBS_ERROR_H
#define AOCLIBS_ERROR_H

#include <a_int.h>
#include <stdio.h>

typedef struct {
        int code;
        const char *msg;
} Error;

typedef struct { Error err; char    value; }  charerr;
typedef struct { Error err; char*   value; }  cstrerr;
typedef struct { Error err; double   value; }  doubleerr;
typedef struct { Error err; float   value; }  floaterr;
typedef struct { Error err; int   value; }  interr;
typedef struct { Error err; long    value; }  longerr;
typedef struct { Error err; long long    value; }  longlongerr;
typedef struct { Error err; short   value; }  shorterr;
typedef struct { Error err; void*   value; }  voiderr;

#ifdef __STDBOOL_H
typedef struct { Error err; bool    value; }  boolerr;
#endif /* __STDBOOL_H */

#ifdef AOCLIBS_INT_H
typedef struct { Error err; i8  value; }  i8err;
typedef struct { Error err; i16  value; }  i16err;
typedef struct { Error err; i32  value; }  i32err;
typedef struct { Error err; i64  value; }  i64err;

typedef struct { Error err; u8  value; }  u8err;
typedef struct { Error err; u16 value; }  u16err;
typedef struct { Error err; u32 value; }  u32err;
typedef struct { Error err; u64 value; }  u64err;

typedef struct { Error err; float   value; }  f32err;
typedef struct { Error err; double  value; }  f64err;
#endif /* AOCLIBS_INT_H */

#define try(e) do { Error _err = (e); if (_err.code != 0) return _err; } while(0)
#define try_cleanup(e, cleanup) do { \
    Error _err = (e); \
    if (_err.code != 0) { \
        cleanup; \
        return _err; \
    } \
} while(0)

#define try_main(e) do { Error _err = (e); if (_err.code != 0) return _err.code; } while(0)
#define try_main_cleanup(e, cleanup) do { \
    Error _err = (e); \
    if (_err.code != 0) { \
        cleanup; \
        return _err.code; \
    } \
} while(0)

Error err(int code, const char msg[]);
Error ok(void);
Error print_error(Error error, FILE *output);
Error println_error(Error err, FILE *output);

#endif
