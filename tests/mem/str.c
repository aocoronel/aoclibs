#include <aoclibs/error.h>
#include <aoclibs/int.h>
#include <aoclibs/mem/str.h>
#include <tunit.h>

TEST(strlen) {
        char str1[128] = "Hello, world!";
        i32 str_len = strlen(str1);

        ASSERT(str_len == 13);
}

TEST(strcmp) {
        char str1[128] = "Hello, world!";
        char str2[86] = "Hello, world!";
        i32 result = strcmp(str1, str2);

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
        strcat(str1, str2);

        ASSERT(strlen(str1) == 5 + 8);
        ASSERT(str1[strlen(str1)] == '\0');

        ASSERT(strcmp(str1, "Hello, world!") == 0);
}

TEST(strcpy) {
        char str1[128] = "Hello";
        char str2[86] = ", world!";
        strcpy(str1, str2);

        ASSERT(strlen(str1) == strlen(str2));

        ASSERT(strcmp(str1, ", world!") == 0);
}

TEST(strtok) {
        char str1[128] = "Hello, world";

        ei32 pos = strtok(str1, ',');
        ASSERT(pos.value == 5);
}

TEST(strrtok) {
        char str1[128] = "Hello, world";

        ei32 pos = a_strrtok(str1, ',');
        ASSERT(pos.value == 5);
}

TEST(convert) {
        ASSERT(str_to_bool("true", false) == true);
        ASSERT(str_to_double("0.032", (f64)1.0) == (f64)0.032);
        ASSERT(str_to_float("0.032", (f32)1.0) == (f32)0.032);
        ASSERT(str_to_int("1020", (isize)20) == (isize)1020);
}
