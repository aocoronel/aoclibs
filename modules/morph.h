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
 * shared versions: scomp_int.
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

#ifndef MORPH_FILE
#define MORPH_FILE "comptime.h"
#endif

#define MORPH_CONCAT(a, b) MORPH_CONCAT_IMPL(a, b)
#define MORPH_CONCAT_IMPL(a, b) a##b

#ifndef MORPH_USECOMP
#include <stdio.h>
FILE *morph_file;
#define comp(fmt, T, ...)                                                                        \
        ({                                                                                       \
                T __result = (__VA_ARGS__);                                                      \
                fprintf(morph_file, "#define MORPH_RESULT_%d " fmt "\n", __COUNTER__, __result); \
                __result;                                                                        \
        })

#define scomp(fmt, key, T, ...)                                              \
        ({                                                                   \
                T __result = (__VA_ARGS__);                                  \
                fprintf(morph_file, "#define %s " fmt "\n", #key, __result); \
                __result;                                                    \
        })

#define comp_int(...) comp("%d", int, __VA_ARGS__)
#define comp_float(...) comp("%f", float, __VA_ARGS__)
#define comp_double(...) comp("%lf", double, __VA_ARGS__)
#define comp_long(...) comp("%zu", long, __VA_ARGS__)
#define comp_long_long(...) comp("%lld", long long, __VA_ARGS__)
#define comp_size_t(...) comp("%zu", size_t, __VA_ARGS__)
#define comp_cstr(...) comp("%s", char *, __VA_ARGS__)
#define comp_char(...) comp("%c", char, __VA_ARGS__)

#define scomp_int(key, ...) scomp("%d", key, int, __VA_ARGS__)
#define scomp_float(key, ...) scomp("%f", key, float, __VA_ARGS__)
#define scomp_double(key, ...) scomp("%lf", key, double, __VA_ARGS__)
#define scomp_long(key, ...) scomp("%zu", key, long, __VA_ARGS__)
#define scomp_long_long(...) scomp("%lld", key, long long, __VA_ARGS__)
#define scomp_size_t(...) scomp("%zu", key, size_t, __VA_ARGS__)
#define scomp_cstr(key, ...) scomp("%s", key, char *, __VA_ARGS__)
#define scomp_char(key, ...) scomp("%c", key, char, __VA_ARGS__)

#define comp_array(fmt, T, ...)                                                          \
        ({                                                                               \
                T __result[] = { __VA_ARGS__ };                                          \
                size_t __len = sizeof(__result) / sizeof(T);                             \
                fprintf(morph_file, "static " #T " MORPH_RESULT_%d[] = {", __COUNTER__); \
                for (size_t i = 0; i < __len; i++) {                                     \
                        fprintf(morph_file, fmt, __result[i]);                           \
                        if (i < __len - 1) fprintf(morph_file, ",");                     \
                }                                                                        \
                fprintf(morph_file, "};\n");                                             \
                __result;                                                                \
        })

#define scomp_array(fmt, key, T, ...)                                            \
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

#define comp_array_int(...) comp_array("%d", int, __VA_ARGS__)
#define comp_array_float(...) comp_array("%f", float, __VA_ARGS__)
#define comp_array_double(...) comp_array("%lf", double, __VA_ARGS__)
#define comp_array_long(...) comp_array("%zu", long, __VA_ARGS__)
#define comp_array_long_long(...) comp("%lld", long long, __VA_ARGS__)
#define comp_array_size_t(...) comp("%zu", size_t, __VA_ARGS__)
#define comp_array_cstr(...) comp_array("%s", char *, __VA_ARGS__)
#define comp_array_char(...) comp_array("%c", char, __VA_ARGS__)

#define scomp_array_int(key, ...) scomp_array("%d", key, int, __VA_ARGS__)
#define scomp_array_float(key, ...) scomp_array("%f", key, float, __VA_ARGS__)
#define scomp_array_double(key, ...) scomp_array("%lf", key, double, __VA_ARGS__)
#define scomp_array_long(key, ...) scomp_array("%zu", key, long, __VA_ARGS__)
#define scomp_array_long_long(...) scomp("%lld", key, long long, __VA_ARGS__)
#define scomp_array_size_t(...) scomp("%zu", key, size_t, __VA_ARGS__)
#define scomp_array_cstr(key, ...) scomp_array("%s", key, char *, __VA_ARGS__)
#define scomp_array_char(key, ...) scomp_array("%c", key, char, __VA_ARGS__)

#define usecomp(key, _default) _default
#else
#define __COMP_RESULT__ MORPH_CONCAT(MORPH_RESULT_, __COUNTER__)
#define __SCOMP_RESULT__(key) MORPH_CONCAT(key, __COUNTER__)

#define comp(print, T, ...) __COMP_RESULT__
#define scomp(print, key, T, ...) __SCOMP_RESULT__(key)

#define comp_int(...) __COMP_RESULT__
#define comp_float(...) __COMP_RESULT__
#define comp_double(...) __COMP_RESULT__
#define comp_long(...) __COMP_RESULT__
#define comp_long_long(...) __COMP_RESULT__
#define comp_size_t(...) __COMP_RESULT__
#define comp_cstr(...) __COMP_RESULT__
#define comp_char(...) __COMP_RESULT__

#define scomp_int(key, ...) __SCOMP_RESULT__(key)
#define scomp_float(key, ...) __SCOMP_RESULT__(key)
#define scomp_double(key, ...) __SCOMP_RESULT__(key)
#define scomp_long(key, ...) __SCOMP_RESULT__(key)
#define scomp_long_long(key, ...) __SCOMP_RESULT__(key)
#define scomp_size_t(key, ...) __SCOMP_RESULT__(key)
#define scomp_cstr(key, ...) __SCOMP_RESULT__(key)
#define scomp_char(key, ...) __SCOMP_RESULT__(key)

#define comp_array(key, T, ...) __COMP_RESULT__
#define scomp_array(key, T, ...) __SCOMP_RESULT__(key)

#define comp_array_int(...) __COMP_RESULT__
#define comp_array_float(...) __COMP_RESULT__
#define comp_array_double(...) __COMP_RESULT__
#define comp_array_long(...) __COMP_RESULT__
#define comp_array_long_long(...) __COMP_RESULT__
#define comp_array_size_t(...) __COMP_RESULT__
#define comp_array_cstr(...) __COMP_RESULT__
#define comp_array_char(...) __COMP_RESULT__

#define scomp_array_int(key, ...) __SCOMP_RESULT__(key)
#define scomp_array_float(key, ...) __SCOMP_RESULT__(key)
#define scomp_array_double(key, ...) __SCOMP_RESULT__(key)
#define scomp_array_long(key, ...) __SCOMP_RESULT__(key)
#define scomp_array_long_long(key, ...) __SCOMP_RESULT__(key)
#define scomp_array_size_t(key, ...) __SCOMP_RESULT__(key)
#define scomp_array_cstr(key, ...) __SCOMP_RESULT__(key)
#define scomp_array_char(key, ...) __SCOMP_RESULT__(key)

#define usecomp(key, _default) key
#endif

#ifndef MORPH_USECOMP
__attribute__((constructor)) static void __morph_init(void) {
        morph_file = fopen(MORPH_FILE, "w");
}

__attribute__((destructor)) static void __morph_deinit(void) {
        fclose(morph_file);
}
#endif

#endif // AOCLIBS_MORPH_H_
