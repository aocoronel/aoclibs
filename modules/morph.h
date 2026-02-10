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

#ifndef MORPH_FILE
#define MORPH_FILE "comptime.h"
#endif

#define MORPH_CONCAT(a, b) MORPH_CONCAT_IMPL(a, b)
#define MORPH_CONCAT_IMPL(a, b) a##b

#ifndef MORPH_USECOMP
#include <stdio.h>
FILE *morph_file;

#define comp(fmt, key, T, ...)                                               \
        ({                                                                   \
                T __result = (__VA_ARGS__);                                  \
                fprintf(morph_file, "#define %s " fmt "\n", #key, __result); \
                __result;                                                    \
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
#define comp_array_long_long(...) comp("%lld", key, long long, __VA_ARGS__)
#define comp_array_size_t(...) comp("%zu", key, size_t, __VA_ARGS__)
#define comp_array_cstr(key, ...) comp_array("%s", key, char *, __VA_ARGS__)
#define comp_array_char(key, ...) comp_array("%c", key, char, __VA_ARGS__)

#define usecomp(key, _default) _default
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
