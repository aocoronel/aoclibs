#ifndef AOCLIBS_MORPH_H_
#define AOCLIBS_MORPH_H_

/*
 * Morph is a simple and deterministic code generator.
 *
 * The goal is to generate code using C, and writing the instructions as close as C.
 *
 * It doesn't require an external executable to generate code. The only thing you need is a C
 * compiler.
 *
 * Currently Morph has these features:
 *
 * 1. Consistent indentation defined by MORPH_INDENTATION.
 *
 * 2. Generate defines, which includes the result of a given expression or function:
 *
 * This code will create a DEFINE named LOOP_RESULT, which has the value of 30.
 *
 * ```c
 * int loop_10_times(int x) {
 *      for (int i = 0; i < 10; i++) x++;
 *      return x;
 * }
 *
 * int myvar = comp_int(LOOP_RESULT, loop_10_times(20));
 * ```
 *
 * You can also generate a whole array:
 *
 * A length is also available in this case: LOOP_RESULT_LEN.
 *
 * ```c
 * int loop_10_times(int x) {
 *      int y[10] = {0};
 *      for (int i = 0; i < 10; i++) y[x++] = x;
 *      return y;
 * }
 *
 * int *myvar = comp_array_int(LOOP_RESULT, loop_10_times(20));
 * ```
 *
 * Lastly, which a little bit of extra effort, you can also do it with structs.
 *
 * ```c
 * typedef struct {
 *      char *name;
 *      int year;
 * } Car;
 *
 * // We need to generate the string
 * const char *print_car(Car c) {
 *         char *car = malloc(256);
 *         snprintf(car, 256, "(Car){ .name = %s, .int = %d }", c.name, c.year);
 *         return car;
 * }
 *
 * // Returns the struct. The string is expected to be allocated, and is automatically freed.
 * // With this approach, you can reuse this struct to do something else.
 * Car mycar = comp_struct(MY_CAR, Car, print_car, .name = "Sedan Crown", .year = 1955);
 * ```
 *
 * 3. Generate functions, or any code you want:
 *
 * This will generate a function definition for strlen. Note the "//@" which allows you to insert
 * the template inline, instead of having a separate file. This can be modified by changing the
 * MORPH_COMMENT define.
 *
 * This will generate to a file named "comptime.h", which can be modified.
 *
 * ```c
 * MORPH_GENERATE();
 *
 * //@ int strlen(const char *s);
 * ```
 *
 * You can also loop this one-hundread times.
 *
 * ```c
 * for (int i = 0; i < 100; i++) {
 *      MORPH_GENERATE();
 * }
 *
 * //@ int strlen(const char *s);
 * ```
 *
 * The user can also specify variables to replace the generated code:
 *
 * ```c
 * char *func_name = "strlen";
 * MORPH_GENERATE(BIND_STRING(func_name));
 *
 * //@ int <func_name>(const char *s);
 * ```
 *
 * Reusing the same template is simple:
 *
 * TODO: Conditionals. For instance, `strnlen` takes a second param, but strlen doesn't
 *
 * ```c
 * char *func_name = "strlen";
 * MORPH_GENERATE(BIND_STRING(func_name));
 * MORPH_GENERATE(SET_STRING(func_name, "strnlen"));
 *
 * //@ int <func_name>(const char *s);
 * ```
 *
 * If you ever generate code that may allocate, don't excitate to use `defer`:
 *
 * Defer will add its statements before calling return, or reaching the end of a scope.
 *
 * ```c
 * //@ int allocate_a_million_bytes(char *buff) {
 * //@      buff = malloc(1_000_000);
 * //@      if (buff == NULL) return -1;
 * //@      defer {
 * //@              free(buff);
 * //@      }
 * //@      char *message = "Hello, world!";
 * //@      memcpy(buff, message, strlen(message));
 * //@      printf("%s\n", buff);
 * //@      return 0;
 * //@ }
 * ```
 *
 * 4. Generate macros:
 *
 * You don't have to include the ending '\'
 *
 * ```c
 * char *macro_name = "add";
 * MORPH_DEFINE(BIND_STRING(macro_name));
 *
 * //@ #define <macro_name>(a, b)
 * //@     a + b
 * ```
 *
 * 5. TODO: Generate from stdout.
*/

#define _GNU_SOURCE
#include "../base/rc.h"
#include "../modules/file.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef MORPH_FILE
#define MORPH_FILE "comptime.h"
#endif

#ifndef MORPH_FD
#define MORPH_FD morph_file
#endif

#ifndef MORPH_COMMENT
#define MORPH_COMMENT "//@"
#endif

#ifndef MORPH_INDENTATION
#define MORPH_INDENTATION 4
#endif

FILE *MORPH_FD;

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

typedef enum {
        MORPH_CLOSE_SCOPE,
        MORPH_DEFER,
        MORPH_NONE,
        MORPH_OPEN_SCOPE,
        MORPH_RETURN,
        MORPH_VAR,
} MorphKeyword;

typedef struct {
        const char *name;
        MorphType Type;
        union {
                bool b;
                const char *C;
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
        size_t count;
} Morph;

typedef struct {
        size_t len;
        size_t cap;
        char *data;
} MorphLine;

typedef struct {
        size_t len;
        size_t cap;
        int scope;
        MorphLine *data;
} MorphString;

typedef struct {
        size_t len;
        size_t cap;
        MorphString *data;
} MorphDefer;

#define MORPH_CONCAT(a, b) MORPH_CONCAT_IMPL(a, b)
#define MORPH_CONCAT_IMPL(a, b) a##b

#define MORPH_CHANGE_FILE(name, write)         \
        do {                                   \
                assert(MORPH_FD != NULL);      \
                fclose(MORPH_FD);              \
                MORPH_FD = fopen(name, write); \
        } while (0)

#define comp(fmt, key, T, ...)                                               \
        ({                                                                   \
                T __result = (__VA_ARGS__);                                  \
                fprintf(morph_file, "#define %s " fmt "\n", #key, __result); \
                __result;                                                    \
        })

#define comp_struct(key, T, print, ...)                                \
        ({                                                             \
                T __result = ((T){ __VA_ARGS__ });                     \
                char *__print = (print(__result));                     \
                fprintf(morph_file, "#define %s %s\n", #key, __print); \
                free(__print);                                         \
                __result;                                              \
        })

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

#define comp_int(key, ...) comp("%d", key, int, __VA_ARGS__)
#define comp_float(key, ...) comp("%f", key, float, __VA_ARGS__)
#define comp_double(key, ...) comp("%lf", key, double, __VA_ARGS__)
#define comp_long(key, ...) comp("%zu", key, long, __VA_ARGS__)
#define comp_long_long(...) comp("%lld", key, long long, __VA_ARGS__)
#define comp_size_t(...) comp("%zu", key, size_t, __VA_ARGS__)
#define comp_cstr(key, ...) comp("%s", key, char *, __VA_ARGS__)
#define comp_char(key, ...) comp("%c", key, char, __VA_ARGS__)

#define comp_array_int(key, ...) comp_array("%d", key, int, __VA_ARGS__)
#define comp_array_float(key, ...) comp_array("%f", key, float, __VA_ARGS__)
#define comp_array_double(key, ...) comp_array("%lf", key, double, __VA_ARGS__)
#define comp_array_long(key, ...) comp_array("%zu", key, long, __VA_ARGS__)
#define comp_array_long_long(...) comp_array("%lld", key, long long, __VA_ARGS__)
#define comp_array_size_t(...) comp_array("%zu", key, size_t, __VA_ARGS__)
#define comp_array_cstr(key, ...) comp_array("%s", key, char *, __VA_ARGS__)
#define comp_array_char(key, ...) comp_array("%c", key, char, __VA_ARGS__)

// Generation flags:
#define MORPH_PRINTF_DEFINE (1 << 0)

#define MORPH_GENERATE(...) _MORPH_GENERATE(__FILE__, __LINE__, 0, __VA_ARGS__)
#define MORPH_DEFINE(...) _MORPH_GENERATE(__FILE__, __LINE__, MORPH_PRINTF_DEFINE, __VA_ARGS__)

#define _MORPH_GENERATE(fi, li, flag, ...)                       \
        do {                                                     \
                Morph _vars[] = { __VA_ARGS__ };                 \
                _vars->count = sizeof(_vars) / sizeof(_vars[0]); \
                rc _tmpl = morph_parse(fi, li);                  \
                if (_tmpl.len > 0) {                             \
                        morph_lexer(_tmpl, _vars, flag);         \
                        aoc_da_free(&_tmpl);                     \
                        fputc('\n', MORPH_FD);                   \
                }                                                \
        } while (0)

__attribute__((constructor)) static void __morph_init(void) {
        morph_file = fopen(MORPH_FILE, "w");
}

__attribute__((destructor)) static void __morph_deinit(void) {
        fclose(morph_file);
}

static void morph_printf(MorphLine *line_contents, const Morph vars, const char *specifier) {
        switch (vars.Type) {
        case MORPH_BOOL:
                if (specifier == NULL) specifier = "%s";
                aoc_rc_fmt_append(line_contents, specifier, vars.type.b == true ? "true" : "false");
                break;
        case MORPH_DOUBLE:
                if (specifier == NULL) specifier = "%lf";
                aoc_rc_fmt_append(line_contents, specifier, vars.type.d);
                break;
        case MORPH_FLOAT:
                if (specifier == NULL) specifier = "%f";
                aoc_rc_fmt_append(line_contents, specifier, vars.type.f);
                break;
        case MORPH_INTPTR:
                if (specifier == NULL) specifier = "%zu";
                aoc_rc_fmt_append(line_contents, specifier, vars.type.I);
                break;
        case MORPH_INT:
                if (specifier == NULL) specifier = "%d";
                aoc_rc_fmt_append(line_contents, specifier, vars.type.i);
                break;
        case MORPH_SHORT:
                if (specifier == NULL) specifier = "%d";
                aoc_rc_fmt_append(line_contents, specifier, vars.type.s);
                break;
        case MORPH_LONG:
                if (specifier == NULL) specifier = "%zu";
                aoc_rc_fmt_append(line_contents, specifier, vars.type.l);
                break;
        case MORPH_LONG_LONG:
                if (specifier == NULL) specifier = "%lld";
                aoc_rc_fmt_append(line_contents, specifier, vars.type.ll);
                break;
        case MORPH_SIZE_T:
                if (specifier == NULL) specifier = "%zu";
                aoc_rc_fmt_append(line_contents, specifier, vars.type.S);
                break;
        case MORPH_CHAR:
                if (specifier == NULL) specifier = "%c";
                aoc_rc_fmt_append(line_contents, specifier, vars.type.c);
                break;
        case MORPH_STRING:
                if (specifier == NULL) specifier = "%s";
                aoc_rc_fmt_append(line_contents, specifier, vars.type.C);
                break;
        default:
                fprintf(stderr, "[ERROR] Unsupported Type in %s\n", vars.name);
                break;
        }
}

#define MORPH_MATCH(s) aoc_cstr_has(p, line_end, (s), lcstrlen(s)) == true
MorphKeyword morph_get_keyword(const char *p, const size_t line_end) {
        if (MORPH_MATCH("return")) return MORPH_RETURN;
        if (MORPH_MATCH("defer {")) return MORPH_DEFER;
        if (MORPH_MATCH("{")) return MORPH_OPEN_SCOPE;
        if (MORPH_MATCH("}")) return MORPH_CLOSE_SCOPE;
        if (MORPH_MATCH("<")) return MORPH_VAR;
        return MORPH_NONE;
}

#define MORPH_SHIFT(shift)          \
        do {                        \
                p += shift;         \
                remaining -= shift; \
                off += shift;       \
        } while (0)

static void morph_process_line(Morph *vars,
                               MorphLine *line_contents,
                               const char **p_ptr,
                               size_t *line_end,
                               size_t *offset,
                               int morph_flag) {
        const char *p = *p_ptr;
        size_t remaining = *line_end;
        size_t off = *offset;

        while (remaining > 0) {
                if (*p == '"') {
                        int quote_end = index_of(p + 1, '"');
                        if (quote_end < 0) break;

                        aoc_da_copy(line_contents, p + 1, quote_end - 1);

                        size_t shift = quote_end + 1;
                        MORPH_SHIFT(shift);
                        continue;
                } else if (*p == '`') {
                        int quote_end = index_of(p + 1, '`');
                        if (quote_end < 0) break;

                        aoc_da_copy(line_contents, p + 1, quote_end - 1);

                        size_t shift = quote_end + 1;
                        MORPH_SHIFT(shift);
                        continue;
                } else if (*p == '<') {
                        const char *start = p + 1;
                        int quote_end = index_of_till(start, '>', remaining);
                        if (quote_end < 0) break;

                        const char *end = start + quote_end;
                        int colon_start = index_of_till(start, ':', remaining);
                        const char *colon = (colon_start >= 0) ? start + colon_start : NULL;

                        const char *var_name_start = start;
                        size_t var_name_len = (colon != NULL) ? (colon - start) - 1 :
                                                                (end - start) - 1;
                        const char *fmt_spec = (colon != NULL) ? colon : NULL;
                        size_t fmt_spec_len = (colon != NULL) ? (end - (colon + 1)) : 0;

                        for (size_t i = 0; i < vars->count; i++) {
                                if (strlen(vars[i].name) == var_name_len &&
                                    memcmp(vars[i].name, var_name_start, var_name_len) == 0) {
                                        char *specifier = NULL;
                                        if (fmt_spec != NULL) {
                                                specifier = malloc(fmt_spec_len + 1);
                                                memcpy(specifier, fmt_spec, fmt_spec_len);
                                                specifier[fmt_spec_len] = '\0';
                                        }

                                        morph_printf(line_contents, vars[i], specifier);
                                        if (fmt_spec != NULL) free(specifier);
                                        break;
                                }
                        }
                        size_t shift = quote_end + 1;
                        MORPH_SHIFT(shift);
                        continue;
                } else {
                        if (*p == '\n' && morph_flag & MORPH_PRINTF_DEFINE) {
                                const char *_p = p + 1;
                                if (*_p == ' ') aoc_da_insert(line_contents, '\\');
                        }
                        aoc_da_insert(line_contents, *p);

                        MORPH_SHIFT(1);
                }
        }

        *p_ptr = p;
        *line_end = remaining;
        *offset = off;
}

#define MORPH_LAST_CHAR line_end - 2

#define MORPH_APPEND_INDENT fprintf(MORPH_FD, "%*c", indentation + 1, '\0');

#define MORPH_INSERT_DEFER                                                                       \
        do {                                                                                     \
                foreach (&defer, i) {                                                            \
                        MorphString *defer_string = &defer.data[i];                              \
                        if (defer_string->data == NULL) continue;                                \
                        if (defer_string->scope != scope) continue;                              \
                        foreach (defer_string, j) {                                              \
                                MorphLine defer_line = defer_string->data[j];                    \
                                if (defer_line.data == NULL) continue;                           \
                                                                                                 \
                                fwrite(defer_line.data, sizeof(char), defer_line.len, MORPH_FD); \
                                                                                                 \
                                aoc_da_free(&defer_line);                                        \
                        }                                                                        \
                        aoc_da_free(defer_string);                                               \
                }                                                                                \
        } while (0)

#define MORPH_NEXT_LINE         \
        offset += line_end;     \
        p = tmpl.data + offset; \
        line_end = index_of(p, '\n');

#define MORPH_SKIP_INDENTATION           \
        do {                             \
                while (*p == ' ' && p) { \
                        line_end--;      \
                        offset++;        \
                        p++;             \
                }                        \
        } while (0)

static void morph_lexer(const rc tmpl, Morph *vars, int morph_flag) {
        MorphDefer defer = { 0 };
        MorphKeyword keyword = 0;

        int scope = 0;

        const char *p = tmpl.data;
        size_t line_end = index_of(p, '\n');

        size_t offset = 0;

        while (offset < tmpl.len) {
                MorphLine line_contents = { 0 };
                aoc_da_reserve(&line_contents, line_end);
                memset(line_contents.data, 0, line_contents.cap);

                int indentation = scope * MORPH_INDENTATION;

                MORPH_SKIP_INDENTATION;

                if (p[MORPH_LAST_CHAR] == '}') {
                        MORPH_INSERT_DEFER;
                        if (scope != 0) scope--;
                        indentation = scope * MORPH_INDENTATION;
                } else if (p[MORPH_LAST_CHAR] == '{') {
                        scope++;
                }

                keyword = morph_get_keyword(p, line_end);

                switch (keyword) {
                case MORPH_DEFER:
                        MORPH_NEXT_LINE;

                        int defer_scope = scope - 1;

                        MorphString defer_string = {
                                .data = NULL, .scope = defer_scope, .len = 0, .cap = 0
                        };

                        int brace = 1;
                        while (brace != 0) {
                                MorphLine defer_line = { 0 };
                                aoc_da_reserve(&defer_line, line_end);
                                // defer_line may contain garbage data
                                memset(defer_line.data, 0, defer_line.cap);
                                int indentation = scope * MORPH_INDENTATION;

                                MORPH_SKIP_INDENTATION;

                                if (p[MORPH_LAST_CHAR] == '}') {
                                        brace -= 1;
                                        if (scope != 0) scope--;
                                        if (brace == 0) goto defer_end;
                                        indentation = scope * MORPH_INDENTATION;
                                } else if (p[MORPH_LAST_CHAR] == '{') {
                                        brace += 1;
                                        scope++;
                                }

                                aoc_da_reserve(&defer_line, indentation - MORPH_INDENTATION);
                                for (int i = 0; i < indentation - MORPH_INDENTATION; i++) {
                                        aoc_da_insert_fast(&defer_line, ' ');
                                }

                                morph_process_line(
                                        vars, &defer_line, &p, &line_end, &offset, morph_flag);

                                aoc_da_insert(&defer_string, defer_line);

                                MORPH_NEXT_LINE;
                        }
defer_end:
                        aoc_da_insert(&defer, defer_string);

                        goto next_line;
                case MORPH_RETURN:
                        MORPH_INSERT_DEFER;
                        goto print_and_next_line;
                default:
                        NULL;
                }

print_and_next_line:
                morph_process_line(vars, &line_contents, &p, &line_end, &offset, morph_flag);

                MORPH_APPEND_INDENT;

                fwrite(line_contents.data, sizeof(char), line_contents.len, MORPH_FD);

next_line:
                MORPH_NEXT_LINE;
        }
        fflush(MORPH_FD);
}

static rc morph_parse(const char *file, int line) {
        FILE *fp = fopen(file, "r");
        if (!fp) {
                fprintf(stderr, "Failed to open file %s. %s\n", file, strerror(errno));
                return (rc){};
        }

        rc str = aoc_rc_new(1024 * 1024);
        memset(str.data, 0, str.cap);

        char *buffer = NULL;
        size_t size = 0;
        size_t nread;

        int found_line = 0;
        int in_comment_block = 0;

        int line_count = 0;

        for (; (nread = read_by_delim(&buffer, &size, '\n', fp)) != SIZE_MAX;) {
                line_count++;

                // Goto line
                if (!found_line) {
                        if (line_count == line) found_line = 1;
                        continue;
                }

                if (strstr(buffer, MORPH_COMMENT)) {
                        in_comment_block = 1;
                        char *comment_start = strstr(buffer, MORPH_COMMENT);
                        if (comment_start) {
                                comment_start += 4;
                                aoc_rc_cat(&str, comment_start, aoc_cstrlen(comment_start, nread));
                        }
                        continue;
                }

                if (in_comment_block) {
                        if (strstr(buffer, MORPH_COMMENT) == NULL) {
                                break;
                        }
                        aoc_rc_cat(&str, buffer, aoc_cstrlen(buffer, nread));
                }
        }

        assert(buffer != NULL);
        free(buffer);
        assert(fp != NULL);
        fclose(fp);

        return str;
}

// Convenient macro boilerplate

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
