#ifndef AOCLIBS_ERROR_H
#define AOCLIBS_ERROR_H

/* === Types === */

#include <stdio.h>

#define Ok 0

// clang-format off

/*
 * Stores the error code and its message
*/
typedef struct { int code; const char *msg; } Err;

typedef struct { Err err; char value; }  echar;
typedef struct { Err err; char* value; } estr;
typedef struct { Err err; double value; } edouble;
typedef struct { Err err; float value; } efloat;
typedef struct { Err err; int value; } eint;
typedef struct { Err err; long value; }  elong;
typedef struct { Err err; long long value; }  elonglong;
typedef struct { Err err; short value; } eshort;
typedef struct { Err err; void* value; } evoid;

#ifdef AOCLIBS_TYPES_H
typedef struct { Err err; bool value; } ebool;

typedef struct { Err err; i8 value; } ei8;
typedef struct { Err err; i16 value; } ei16;
typedef struct { Err err; i32 value; } ei32;
typedef struct { Err err; i64 value; } ei64;

typedef struct { Err err; u8 value; } eu8;
typedef struct { Err err; u16 value; } eu16;
typedef struct { Err err; u32 value; } eu32;
typedef struct { Err err; u64 value; } eu64;

typedef struct { Err err; f32  value; } ef32;
typedef struct { Err err; f64 value; } ef64;

typedef struct { Err err; isize value; } eisize;
typedef struct { Err err; usize value; } eusize;
#endif

// clang-format on

/* === Functions === */

/*
 * Creates an Err type
*/
Err werr(int code, const char msg[]);

/*
 * Returns success Err
 *
 * code: 0
 *
 * msg: "OK"
*/
Err ok(void);

/*
 * Return Err + Type
*/

echar werr_char(Err err, char value);
estr werr_str(Err err, char *value);
edouble werr_double(Err err, double value);
efloat werr_float(Err err, float value);
eint werr_int(Err err, int value);
elong werr_long(Err err, long value);
elonglong werr_longlong(Err err, long long value);
eshort werr_short(Err err, short value);
evoid werr_void(Err err, void *value);

#ifdef AOCLIBS_TYPES_H
ebool werr_bool(Err err, bool value);

ei8 werr_i8(Err err, i8 value);
ei16 werr_i16(Err err, i16 value);
ei32 werr_i32(Err err, i32 value);
ei64 werr_i64(Err err, i64 value);

eu8 werr_u8(Err err, u8 value);
eu16 werr_u16(Err err, u16 value);
eu32 werr_u32(Err err, u32 value);
eu64 werr_u64(Err err, u64 value);
ef32 werr_f32(Err err, float value);
ef64 werr_f64(Err err, double value);

eisize werr_isize(Err err, intptr_t value);
eusize werr_usize(Err err, uintptr_t value);
#endif

#undef panic

/*
 * Used to panic, when an unreachable code runs
*/
#define unreachable() _panic(__FILE__, __LINE__, __func__, "unreachable code")

#define panic(msg) _panic(__FILE__, __LINE__, __func__, msg)

/*
 * Prints message, including metadata and aborts the program
 *
 * Should not be used directly. Use the panic macro, instead.
*/
_Noreturn void _panic(const char *__file, int __line, const char *__func,
                      const char *msg);

/*
 * Prints error message
*/
Err rerr(Err error, FILE *output);

/*
 * Prints error message, and appends a new line
*/
Err rerrln(Err err, FILE *output);

/* === Asserts === */

#ifdef NDEBUG
#define assert_ok(e) ((void)0)
#define assert_err(e) ((void)0)
#else

/*
 * Asserts the error code is success (code == 0)
 */
#define assert_ok(e)                                                   \
        do {                                                           \
                if ((e).code != Ok) {                                  \
                        fprintf(stderr,                                \
                                "Assertion failed: expected success\n" \
                                "  got: %d : %s\n"                     \
                                "  at %s:%d (%s)\n",                   \
                                (e).code, (e).msg, __FILE__, __LINE__, \
                                __func__);                             \
                        abort();                                       \
                }                                                      \
        } while (0)

/*
 * Asserts the error code is an error (code != 0)
 */
#define assert_err(e)                                                  \
        do {                                                           \
                if ((e).code == Ok) {                                  \
                        fprintf(stderr,                                \
                                "Assertion failed: expected error\n"   \
                                "  got: %d : %s\n"                     \
                                "  at %s:%d (%s)\n",                   \
                                (e).code, (e).msg, __FILE__, __LINE__, \
                                __func__);                             \
                        abort();                                       \
                }                                                      \
        } while (0)
#endif

#endif /* AOCLIBS_ERROR_H */
