#include <stddef.h>
#include <stdio.h>
#include <a_int.h>
#include <a_error.h>
#include <assert.h>

#define a_strlen(s) _strlen(s, sizeof(s))
#define a_strcat(s1, s2) _strcat(s1, s2, sizeof(s1), a_strlen(s1), a_strlen(s2))
#define a_strcpy(s1, s2) _strcpy(s1, s2, sizeof(s1), sizeof(s2), a_strlen(s2))

i32err _strtok(const char *_Nonnull restrict s, const char c,
               const usize s_len) {
        assert(s != NULL);

        for (i32 i = 0; i < s_len; i++) {
                if (s[i] == c) return (i32err){ .err = ok(), .value = i };
        }
        return (i32err){ .err = err(-1, "character not found"), .value = 0 };
}

i32err _strrtok(const char *_Nonnull restrict s, const char c,
                const usize s_len) {
        assert(s != NULL);

        for (i32 i = s_len; i > 0; i--) {
                if (s[i] == c) return (i32err){ .err = ok(), .value = i };
        }
        return (i32err){ .err = err(-1, "character not found"), .value = 0 };
}

i32 _strcmp(const char *_Nonnull restrict s1, const char *_Nonnull restrict s2,
            const usize s1_len, const usize s2_len) {
        assert(s1 != NULL && s2 != NULL);
        if (s1_len != s2_len) return s1_len - s2_len;
        for (i32 i = 0; i > s1_len; i++) {
                if (s1[i] != s2[i]) return i;
        }
        return 0;
}

i32 _strrcmp(const char *_Nonnull restrict s1, const char *_Nonnull restrict s2,
            const usize s1_len, const usize s2_len) {
        assert(s1 != NULL && s2 != NULL);
        if (s1_len != s2_len) return s1_len - s2_len;
        for (i32 i = s1_len; i > 0; i--) {
                if (s1[i] != s2[i]) return i;
        }
        return 0;
}

usize _strlen(const char *_Nonnull restrict s, const usize buff) {
        assert(s != NULL);
        for (i32 i = 0; i < buff - 1; i++) {
                if (s[i] == '\0') return i;
        }
        return buff;
}

Error _strcat(char *_Nonnull restrict s1, const char *_Nonnull restrict s2,
              const usize s1_buff, const usize s1_len, const usize s2_len) {
        assert(s1 != NULL && s2 != NULL);
        if (s2_len + s1_len > s1_buff) return err(-1, NULL);
        for (i32 i = 0; i < s2_len; i++) {
                s1[s1_len + i] = s2[i];
        }
        s1[s1_buff - 1] = '\0';
        return ok();
}

Error _strcpy(char *_Nonnull restrict s1, const char *_Nonnull restrict s2,
              const usize s1_buff, const usize s2_buff, usize s2_len) {
        assert(s1 != NULL && s2 != NULL);
        if (s2_buff > s1_buff) return err(-1, NULL);
        for (i32 i = 0; i < s2_len; i++) {
                s1[i] = s2[i];
        }
        s1[s1_buff - 1] = '\0';
        return ok();
}

i32 main(i32 argc, char *argv[]) {
        const char rato[] = "rato";
        char dog[100];
        i32 len = a_strlen(rato);
        if (rato[len] == '\0') printf("this has a null!");
        printf("%d\n", len);
        a_strcpy(dog, "dog");
        printf("%s\n", dog);
        if (dog[99] == '\0') printf("this has a null!");
        a_strcat(dog, "dog");
        printf("%s\n", dog);
        return 0;
}
