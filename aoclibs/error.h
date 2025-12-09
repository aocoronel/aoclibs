#ifndef AOCLIBS_ERROR_H
#define AOCLIBS_ERROR_H

/* === Types === */

#include <aoclibs/int.h>
#include <stdio.h>

#define ERR_FAIL -1
#define ERR_SUCCESS 0

// clang-format off

/*
 * Stores the error code and its message
*/
typedef struct { i32 code; const char *msg; } Err;

typedef struct { Err err; char value; }  char_e;
typedef struct { Err err; char* value; } str_e;
typedef struct { Err err; double value; } double_e;
typedef struct { Err err; float value; } float_e;
typedef struct { Err err; int value; } int_e;
typedef struct { Err err; long value; }  long_e;
typedef struct { Err err; long long value; }  longlong_e;
typedef struct { Err err; short value; } short_e;
typedef struct { Err err; void* value; } void_e;

// stdbool.h
#ifdef __STDBOOL_H
typedef struct { Err err; bool value; } boolerr;
#endif /* __STDBOOL_H */

typedef struct { Err err; i8 value; } i8_e;
typedef struct { Err err; i16 value; } i16_e;
typedef struct { Err err; i32 value; } i32_e;
typedef struct { Err err; i64 value; } i64_e;

typedef struct { Err err; u8 value; } u8_e;
typedef struct { Err err; u16 value; } u16_e;
typedef struct { Err err; u32 value; } u32_e;
typedef struct { Err err; u64 value; } u64_e;

typedef struct { Err err; f32  value; } f32_e;
typedef struct { Err err; f64 value; } f64_e;

typedef struct { Err err; isize value; } isize_e;
typedef struct { Err err; usize value; } usize_e;

// clang-format on

/* === Functions === */

/*
 * Creates an Err type
*/
Err err(int code, const char msg[]);

/*
 * Returns success Err
 *
 * code: 0
 *
 * msg: "OK"
*/
Err ok(void);

#undef panic

/*
 * Used to panic, when an unreachable code runs
*/
#define unreachable() _panic(__FILE__, __LINE__, __func__, "unreachable code")

#define panic(msg) _panic(__FILE__, __LINE__, __func__, msg)

/*
 * Prints message, including metadata and aborts the program
 *
 * Should not be used directly. Use panic, instead.
*/
_Noreturn void _panic(const char *__file, int __line, const char *__func,
                      const char *msg);

/*
 * Prints error message
*/
Err err_print(Err error, FILE *output);

/*
 * Prints error message, and appends a new line
*/
Err err_println(Err err, FILE *output);

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
                if ((e).code != 0) {                                   \
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
                if ((e).code == 0) {                                   \
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
