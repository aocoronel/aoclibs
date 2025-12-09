#include <aoclibs/error.h>
#include <aoclibs/int.h>
#include <aoclibs/mem/str.h>
#include <tunit.h>

TEST(strlen) {
        char str1[128] = "Hello, world!";
        i32 str_len = a_strlen(str1);

        ASSERT(str_len == 13);
}

TEST(strcmp) {
        char str1[128] = "Hello, world!";
        char str2[86] = "Hello, world!";
        i32 result = a_strcmp(str1, str2);

        ASSERT(result == 0);
}

TEST(strrcmp) {
        char str1[128] = "Hello, world!";
        char str2[86] = "Hello, world!";
        i32 result = a_strrcmp(str1, str2);

        ASSERT(result == 0);
}

TEST(strcat) {
        char str1[128] = "Hello";
        char str2[86] = ", world!";
        a_strcat(str1, str2);

        ASSERT(a_strlen(str1) == 5 + 8);
        ASSERT(str1[a_strlen(str1)] == '\0');

        ASSERT(a_strcmp(str1, "Hello, world!") == 0);
}

TEST(strcpy) {
        char str1[128] = "Hello";
        char str2[86] = ", world!";
        a_strcpy(str1, str2);

        ASSERT(a_strlen(str1) == a_strlen(str2));

        ASSERT(a_strcmp(str1, ", world!") == 0);
}

TEST(strtok) {
        char str1[128] = "Hello, world";

        i32_e pos = a_strtok(str1, ',');
        ASSERT(pos.value == 5);
}

TEST(strrtok) {
        char str1[128] = "Hello, world";

        i32_e pos = a_strrtok(str1, ',');
        ASSERT(pos.value == 5);
}

TEST(convert) {
        ASSERT(str_to_bool("true", false) == true);
        ASSERT(str_to_double("0.032", (f64)1.0) == (f64)0.032);
        ASSERT(str_to_float("0.032", (f32)1.0) == (f32)0.032);
        ASSERT(str_to_int("1020", (isize)20) == (isize)1020);
}
