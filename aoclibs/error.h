#ifndef AOCLIBS_ERROR_H
#define AOCLIBS_ERROR_H

/* === Types === */

#include <aoclibs/attributes.h>
#include <aoclibs/int.h>
#include <stdio.h>

// clang-format off

/*
 * Stores the error code and its message
*/
typedef struct { int code; const char *_Nullable msg; } Err;

typedef struct { Err err; char value; }  echar;
typedef struct { Err err; char* value; } estr;
typedef struct { Err err; void* value; } evoid;
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

// clang-format on

// === Error Enum ===

enum {
#define E(name, msg) name,
        Ok = 0,
#include <aoclibs/__errno.h>
#undef E
        _ErrMax
} FN_PACKED;

extern const char *const ErrNo[_ErrMax];

#define E(name, msg) extern const Err Err##name;
#include <aoclibs/__errno.h>
#undef E

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
evoid werr_void(Err err, void *value);
ebool werr_bool(Err err, bool value);

ei8 werr_i8(Err err, i8 value);
ei16 werr_i16(Err err, i16 value);
ei32 werr_i32(Err err, i32 value);
ei64 werr_i64(Err err, i64 value);

eu8 werr_u8(Err err, u8 value);
eu16 werr_u16(Err err, u16 value);
eu32 werr_u32(Err err, u32 value);
eu64 werr_u64(Err err, u64 value);

ef32 werr_f32(Err err, f32 value);
ef64 werr_f64(Err err, f64 value);

eisize werr_isize(Err err, intptr_t value);
eusize werr_usize(Err err, uintptr_t value);

/*
 * Used to panic, when an unreachable code runs
*/
#define unreachable() _panic(__FILE__, __LINE__, __func__, "unreachable code")

#undef panic
#define panic(msg) _panic(__FILE__, __LINE__, __func__, msg)

/*
 * Prints message, including metadata and aborts the program
 *
 * Should not be used directly. Use the panic macro, instead.
*/
void _panic(const char *__file, int __line, const char *__func,
                      const char *msg) FN_ABORTS;

/*
 * Prints error message
 *
 * Failure: errno << fprintf
*/
Err read_err(Err err, FILE *_Nonnull fd);

/*
 * Prints error message, and appends a new line
 *
 * Failure: errno << fprintf
*/
Err read_errln(Err err, FILE *_Nonnull fd);

#define ErrErrno (Err){.code = errno, .msg = strerror(errno)}

#define DEFINE_ERR(def_name, err_code)                \
        const Err Err##err_code = { .code = err_code, \
                                    .msg = def_name[err_code] }

/* === Asserts === */

#ifdef NDEBUG
#define ASSERT_OK(e) ((void)0)
#define ASSERT_ERR(e) ((void)0)
#else

/*
 * Asserts the error code is success (code == 0)
 */
#define ASSERT_OK(e)                                                   \
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
#define ASSERT_ERR(e)                                                  \
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
