#include <a_mem_fn_strcmp.h>
#include <a_types.h>
#include <assert.h>
#include <stddef.h>

i32 _strcmp(const char *s1, const char *s2, const usize s1_len,
            const usize s2_len) {
        assert(s1 != NULL && s2 != NULL);

        usize i = 0;

        while (i < s1_len && i < s2_len) {
                if (s1[i] != s2[i]) {
                        return (u8)s1[i] - (u8)s2[i];
                }
                if (s1[i] == '\0') {
                        return 0;
                }
                i++;
        }
        if (i == s1_len && i == s2_len) {
                return 0;
        }
        if (i == s1_len) {
                return -s2[i];
        }
        return s1[i];
}

i32 _strrcmp(const char *s1, const char *s2, const usize s1_len,
             const usize s2_len) {
        assert(s1 != NULL && s2 != NULL);

        usize i = 0;
        usize s1_pos = s1_len > 0 ? s1_len - 1 : 0;
        usize s2_pos = s2_len > 0 ? s2_len - 1 : 0;

        while (s1_pos != (usize)-1 && s2_pos != (usize)-1) {
                if (s1[s1_pos] != s2[s2_pos]) {
                        return (u8)s1[s1_pos] - (u8)s2[s2_pos];
                }
                if (s1_pos == 0 || s2_pos == 0) {
                        break;
                }
                s1_pos--;
                s2_pos--;
        }
        if (s1_pos == 0 && s2_pos == 0) {
                return 0;
        }
        if (s1_pos == 0) {
                return -s2[s2_pos];
        }
        return s1[s1_pos];
}
