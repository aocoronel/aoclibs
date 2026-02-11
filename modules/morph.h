#ifndef AOCLIBS_MORPH_H_
#define AOCLIBS_MORPH_H_

/*
 * Morph is a simple way to achieve compile-time in C, without overcomplicating anything. This
 * is a standalone, and doesn't depend in anything else from AOCLIBS.
 *
 * It doesn't require an external executable to generate code. The only thing you need is a C
 * compiler.
 *
 * To use it, you can create a comptime expression:
 *
 * int loop_10_times(int x) {
 *      for (int i = 0; i < 10; i++) x++;
 *      return x;
 * }
 * int myvar = comp_int(loop_10_times(20)); // returns 30
 *
 * These macros won't break your code, and will allow you to run code at runtime. After running this,
 * a file called "comptime.h" will be generated with the return value.
 * If you now try to compile the program again, but with the MORPH_USECOMP define, the for loop is
 * never compiled, since its result is cached.
 *
 * What if you want to store a value in a global variable? Instead of using comp_int, you use the
 * shared versions: comp_int.
 * int myvar = comp_int(FOR_LOOP_RESULT, loop_10_times(20)); // returns 30
 * Now you can use the result several times across your code by using: usecomp.
 *
 * int x = usecomp(FOR_LOOP_RESULT, 0);
 *                 ^                ^
 *                 define           fallback value
 *
 * Because, the C compiler will block you from compiling the code, if FOR_LOOP_RESULT is not yet
 * generated, and you also haven't provided the expression to generate it (this is a limitation for
 * global variables), you have to provide a fallback value.
*/

#ifndef MORPH_FD
#define MORPH_FILE "comptime.h"
#define MORPH_FD morph_file
#endif

#define MORPH_COMMENT "//@"

#define _GNU_SOURCE
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *MORPH_FD;

#define MORPH_CHANGE_FILE(name, write)         \
        do {                                   \
                assert(MORPH_FD != NULL);      \
                fclose(MORPH_FD);              \
                MORPH_FD = fopen(name, write); \
        } while (0)

typedef enum {
        MORPH_BOOL,
        MORPH_CHAR,
        MORPH_DOUBLE,
        MORPH_FLOAT,
        MORPH_INT,
        MORPH_INTPTR,
        MORPH_LONG,
        MORPH_LONG_LONG,
        MORPH_SHORT,
        MORPH_SIZE_T,
        MORPH_STRING,
} MorphType;

typedef struct {
        const char *name;
        MorphType Type;
        union {
                bool b;
                char *C;
                char c;
                double d;
                float f;
                int i;
                intptr_t I;
                long l;
                long long ll;
                short s;
                size_t S;
        } type;
} Morph;

#define MORPH_CONCAT(a, b) MORPH_CONCAT_IMPL(a, b)
#define MORPH_CONCAT_IMPL(a, b) a##b

#ifndef MORPH_USECOMP

#define comp(fmt, key, T, ...)                                               \
        ({                                                                   \
                T __result = (__VA_ARGS__);                                  \
                fprintf(morph_file, "#define %s " fmt "\n", #key, __result); \
                __result;                                                    \
        })

#define comp_struct(key, T, print, ...)                                \
        ({                                                             \
                T __result = ((T){ __VA_ARGS__ });                     \
                const char *__print = (print(__result));               \
                fprintf(morph_file, "#define %s %s\n", #key, __print); \
                __result;                                              \
        })

#define comp_int(key, ...) comp("%d", key, int, __VA_ARGS__)
#define comp_float(key, ...) comp("%f", key, float, __VA_ARGS__)
#define comp_double(key, ...) comp("%lf", key, double, __VA_ARGS__)
#define comp_long(key, ...) comp("%zu", key, long, __VA_ARGS__)
#define comp_long_long(...) comp("%lld", key, long long, __VA_ARGS__)
#define comp_size_t(...) comp("%zu", key, size_t, __VA_ARGS__)
#define comp_cstr(key, ...) comp("%s", key, char *, __VA_ARGS__)
#define comp_char(key, ...) comp("%c", key, char, __VA_ARGS__)

#define comp_array(fmt, key, T, ...)                                             \
        ({                                                                       \
                T __result[] = { __VA_ARGS__ };                                  \
                size_t __len = sizeof(__result) / sizeof(T);                     \
                fprintf(morph_file, "static " #T " %s[] = {", key);              \
                for (size_t i = 0; i < __len; i++) {                             \
                        fprintf(morph_file, fmt, __result[i]);                   \
                        if (i < __len - 1) fprintf(morph_file, ",");             \
                }                                                                \
                fprintf(morph_file, "};\n");                                     \
                fprintf(morph_file, "#define %s_LEN %zu\n", __COUNTER__, __len); \
                __result;                                                        \
        })

#define comp_array_int(key, ...) comp_array("%d", key, int, __VA_ARGS__)
#define comp_array_float(key, ...) comp_array("%f", key, float, __VA_ARGS__)
#define comp_array_double(key, ...) comp_array("%lf", key, double, __VA_ARGS__)
#define comp_array_long(key, ...) comp_array("%zu", key, long, __VA_ARGS__)
#define comp_array_long_long(...) comp_array("%lld", key, long long, __VA_ARGS__)
#define comp_array_size_t(...) comp_array("%zu", key, size_t, __VA_ARGS__)
#define comp_array_cstr(key, ...) comp_array("%s", key, char *, __VA_ARGS__)
#define comp_array_char(key, ...) comp_array("%c", key, char, __VA_ARGS__)

#define usecomp(key, _default) _default

#define MORPH_GENERATE(...) _MORPH_GENERATE(__FILE__, __LINE__, __VA_ARGS__)
#else
#define __COMP_RESULT__ MORPH_CONCAT(MORPH_RESULT_, __COUNTER__)
#define __comp_RESULT__(key) MORPH_CONCAT(key, __COUNTER__)

#define comp(print, key, T, ...) __comp_RESULT__(key)

#define comp_int(key, ...) __comp_RESULT__(key)
#define comp_float(key, ...) __comp_RESULT__(key)
#define comp_double(key, ...) __comp_RESULT__(key)
#define comp_long(key, ...) __comp_RESULT__(key)
#define comp_long_long(key, ...) __comp_RESULT__(key)
#define comp_size_t(key, ...) __comp_RESULT__(key)
#define comp_cstr(key, ...) __comp_RESULT__(key)
#define comp_char(key, ...) __comp_RESULT__(key)

#define comp_array(key, T, ...) __comp_RESULT__(key)

#define comp_array_int(key, ...) __comp_RESULT__(key)
#define comp_array_float(key, ...) __comp_RESULT__(key)
#define comp_array_double(key, ...) __comp_RESULT__(key)
#define comp_array_long(key, ...) __comp_RESULT__(key)
#define comp_array_long_long(key, ...) __comp_RESULT__(key)
#define comp_array_size_t(key, ...) __comp_RESULT__(key)
#define comp_array_cstr(key, ...) __comp_RESULT__(key)
#define comp_array_char(key, ...) __comp_RESULT__(key)

#define usecomp(key, _default) key

#define MORPH_GENERATE(...)
#endif

#ifndef MORPH_USECOMP
__attribute__((constructor)) static void __morph_init(void) {
        morph_file = fopen(MORPH_FILE, "w");
}

__attribute__((destructor)) static void __morph_deinit(void) {
        fclose(morph_file);
}
#endif

static void morph_print(const char *tmpl, Morph *vars, size_t var_count) {
        for (const char *p = tmpl; *p; ++p) {
                if (*p == '"') {
                        ++p;
                        while (*p && *p != '"') {
                                fputc(*p, MORPH_FD);
                                ++p;
                        }
                        ++p;
                }
                if (*p == '`') {
                        ++p;
                        while (*p && *p != '`') {
                                fputc(*p, MORPH_FD);
                                ++p;
                        }
                        ++p;
                }
                if (*p == '<') {
                        const char *start = ++p;
                        const char *colon = NULL;
                        const char *end = NULL;

                        while (*p && *p != '>') {
                                if (*p == ':') {
                                        colon = p;
                                }
                                p++;
                        }
                        if (*p == '>') {
                                end = p;
                        }

                        const char *var_name_start = start;
                        size_t var_name_len = colon ? colon - start : end - start;
                        const char *fmt_spec = colon ? colon + 1 : NULL;
                        size_t fmt_spec_len = colon ? end - (colon + 1) : 0;

                        for (size_t i = 0; i < var_count; i++) {
                                if (strlen(vars[i].name) == var_name_len &&
                                    strncmp(vars[i].name, var_name_start, var_name_len) == 0) {
                                        const char *specifier = fmt_spec;
                                        if (specifier) {
                                                char *tmp = malloc(fmt_spec_len + 1);
                                                strncpy(tmp, fmt_spec, fmt_spec_len);
                                                tmp[fmt_spec_len] = '\0';
                                                specifier = tmp;
                                                fprintf(MORPH_FD, specifier, vars[i].type.d);
                                                free(tmp);
                                                p = end;
                                                continue;
                                        }

                                        switch (vars[i].Type) {
                                        case MORPH_BOOL:
                                                fprintf(MORPH_FD, "%s",
                                                        vars[i].type.b == true ? "true" : "false");
                                                break;
                                        case MORPH_DOUBLE:
                                                fprintf(MORPH_FD, "%lf", vars[i].type.d);
                                                break;
                                        case MORPH_FLOAT:
                                                fprintf(MORPH_FD, "%f", vars[i].type.f);
                                                break;
                                        case MORPH_INTPTR:
                                                fprintf(MORPH_FD, "%zu", vars[i].type.I);
                                                break;
                                        case MORPH_INT:
                                                fprintf(MORPH_FD, "%d", vars[i].type.i);
                                                break;
                                        case MORPH_SHORT:
                                                fprintf(MORPH_FD, "%d", vars[i].type.s);
                                                break;
                                        case MORPH_LONG:
                                                fprintf(MORPH_FD, "%zu", vars[i].type.l);
                                                break;
                                        case MORPH_LONG_LONG:
                                                fprintf(MORPH_FD, "%lld", vars[i].type.ll);
                                                break;
                                        case MORPH_SIZE_T:
                                                fprintf(MORPH_FD, "%zu", vars[i].type.S);
                                                break;
                                        case MORPH_CHAR:
                                                fprintf(MORPH_FD, "%c", vars[i].type.c);
                                                break;
                                        case MORPH_STRING:
                                                fprintf(MORPH_FD, "%s", vars[i].type.C);
                                                break;
                                        default:
                                                fprintf(stderr, "[ERROR] Unsupported Type in %s\n",
                                                        vars[i].name);
                                                break;
                                        }
                                        break;
                                }
                        }
                } else {
                        fputc(*p, MORPH_FD);
                }
        }
}

static char *morph_parse(const char *file, int line) {
        FILE *fp = fopen(file, "r");
        if (!fp) {
                fprintf(stderr, "Failed to open file: %s\n", file);
                return NULL;
        }

        char *str = malloc(1024 * 1024);
        *str = '\0';

        char *buffer = NULL;
        size_t size = 0;
        ssize_t nread;

        int found_line = 0;
        int in_comment_block = 0;

        int line_count = 0;

        while (getline(&buffer, &size, fp) != -1) {
                line_count++;
                if (!found_line) {
                        if (line_count == line) {
                                found_line = 1;
                        }
                        continue;
                }

                if (strstr(buffer, MORPH_COMMENT)) {
                        in_comment_block = 1;
                        char *comment_start = strstr(buffer, MORPH_COMMENT);
                        if (comment_start) {
                                comment_start += 4;
                                strcat(str, comment_start);
                        }
                        continue;
                }

                if (in_comment_block) {
                        if (strstr(buffer, MORPH_COMMENT) == NULL) {
                                break;
                        }
                        strcat(str, buffer);
                }
        }

        assert(buffer != NULL);
        free(buffer);
        assert(fp != NULL);
        fclose(fp);

        return str;
}

#define _MORPH_GENERATE(fi, li, ...)                              \
        do {                                                      \
                Morph _vars[] = { __VA_ARGS__ };                  \
                size_t _count = sizeof(_vars) / sizeof(_vars[0]); \
                char *_tmpl = morph_parse(fi, li);                \
                if (_tmpl) {                                      \
                        morph_print(_tmpl, _vars, _count);        \
                        free(_tmpl);                              \
                }                                                 \
                fputc('\n', MORPH_FD);                            \
        } while (0)

#define SET_LONG(x, y)                                           \
        (Morph) {                                                \
                .name = #x, .Type = MORPH_LONG, .type.l = (x, y) \
        }

#define BIND_LONG(x)                                          \
        (Morph) {                                             \
                .name = #x, .Type = MORPH_LONG, .type.l = (x) \
        }

#define SET_LONG_LONG(x, y)                                             \
        (Morph) {                                                       \
                .name = #x, .Type = MORPH_LONG_LONG, .type.ll = (x = y) \
        }

#define BIND_LONG_LONG(x)                                           \
        (Morph) {                                                   \
                .name = #x, .Type = MORPH_LONG_LONG, .type.ll = (x) \
        }

#define SET_SIZE_T(x, y)                                            \
        (Morph) {                                                   \
                .name = #x, .Type = MORPH_SIZE_T, .type.S = (x = y) \
        }

#define BIND_SIZE_T(x)                                          \
        (Morph) {                                               \
                .name = #x, .Type = MORPH_SIZE_T, .type.S = (x) \
        }

#define SET_CHAR(x, y)                                            \
        (Morph) {                                                 \
                .name = #x, .Type = MORPH_CHAR, .type.c = (x = y) \
        }

#define BIND_CHAR(x)                                          \
        (Morph) {                                             \
                .name = #x, .Type = MORPH_CHAR, .type.c = (x) \
        }

#define SET_INT(x, y)                                            \
        (Morph) {                                                \
                .name = #x, .Type = MORPH_INT, .type.i = (x = y) \
        }

#define BIND_INT(x)                                          \
        (Morph) {                                            \
                .name = #x, .Type = MORPH_INT, .type.i = (x) \
        }

#define SET_INTPTR(x, y)                                         \
        (Morph) {                                                \
                .name = #x, .Type = MORPH_INT, .type.I = (x = y) \
        }

#define BIND_INTPTR(x)                                       \
        (Morph) {                                            \
                .name = #x, .Type = MORPH_INT, .type.I = (x) \
        }

#define SET_SHORT(x, y)                                            \
        (Morph) {                                                  \
                .name = #x, .Type = MORPH_SHORT, .type.s = (x = y) \
        }

#define BIND_SHORT(x)                                          \
        (Morph) {                                              \
                .name = #x, .Type = MORPH_SHORT, .type.s = (x) \
        }

#define SET_STRING(x, y)                                            \
        (Morph) {                                                   \
                .name = #x, .Type = MORPH_STRING, .type.C = (x = y) \
        }

#define BIND_STRING(x)                                          \
        (Morph) {                                               \
                .name = #x, .Type = MORPH_STRING, .type.C = (x) \
        }

#define SET_BOOL(x, y)                                            \
        (Morph) {                                                 \
                .name = #x, .Type = MORPH_BOOL, .type.b = (x = y) \
        }

#define BIND_BOOL(x)                                          \
        (Morph) {                                             \
                .name = #x, .Type = MORPH_BOOL, .type.b = (x) \
        }

#define SET_FLOAT(x, y)                                            \
        (Morph) {                                                  \
                .name = #x, .Type = MORPH_FLOAT, .type.f = (x = y) \
        }

#define BIND_FLOAT(x)                                          \
        (Morph) {                                              \
                .name = #x, .Type = MORPH_FLOAT, .type.f = (x) \
        }

#define SET_DOUBLE(x, y)                                            \
        (Morph) {                                                   \
                .name = #x, .Type = MORPH_DOUBLE, .type.d = (x = y) \
        }

#define BIND_DOUBLE(x)                                          \
        (Morph) {                                               \
                .name = #x, .Type = MORPH_DOUBLE, .type.d = (x) \
        }

#endif // AOCLIBS_MORPH_H_
