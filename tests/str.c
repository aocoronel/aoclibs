#include <aoclibs/common.h>
#include <aoclibs/mem/convert.h>
#include <aoclibs/mem/str.h>
#include <aoclibs/tunit.h>

TEST(mem_strlen) {
        char str1[128] = "Hello, world!";
        i32 str_len = mem_strlen(str1);

        TASSERT(str_len == 13, "Hello, world! should be 13 bytes long");
}

TEST(mem_streq) {
        char str1[128] = "Hello, world!";
        char str2[86] = "Hello, world!";
        i32 result = mem_streq(str1, str2);

        TASSERT(result == 0, "strings must be equal");
}

TEST(mem_strreq) {
        char str1[128] = "Hello, world!";
        char str2[86] = "Hello, world!";
        i32 result = mem_strreq(str1, str2);

        TASSERT(result == 0, "strings must be equal");
}

TEST(mem_strcat) {
        char str1[128] = "Hello";
        char str2[86] = ", world!";
        mem_strcat(str1, str2);

        TASSERT(mem_strlen(str1) == 5 + 8,
                "Hello, world! must be 13 bytes long");
        TASSERT(str1[mem_strlen(str1)] == '\0',
                "str1 should be null terminated");

        TASSERT(mem_streq(str1, "Hello, world!") == 0, "strings must be equal");
}

TEST(mem_strcpy) {
        char str1[128] = "Hello";
        char str2[86] = ", world!";
        mem_strcpy(str1, str2);

        TASSERT(mem_strlen(str1) == mem_strlen(str2),
                "str1 should be exactly like str2");

        TASSERT(mem_streq(str1, ", world!") == 0, "str1 should be ', world!'");
}

TEST(mem_strfind) {
        char str1[128] = "Hello, world";

        ei32 pos = mem_strfind(str1, ',');
        TASSERT(pos.value == 5, "the ',' should be at the 5th index");
}

TEST(mem_strrfind) {
        char str1[128] = "Hello, world";

        ei32 pos = mem_strrfind(str1, ',');
        TASSERT(pos.value == 5, "the ',' should be at the 5th index");
}

TEST(convert) {
        TASSERT(mem_strtob("true", false) == true, "returns true");
        TASSERT(mem_strtod("0.032", (f64)1.0) == (f64)0.032,
                "returns 0.032 as f64");
        TASSERT(mem_strtof("0.032", (f32)1.0) == (f32)0.032,
                "returns 0.032 as f32");
        TASSERT(mem_strtol("1020", (isize)20) == (isize)1020, "returns 1020");
}
