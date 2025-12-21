#include <aoclibs/common.h>
#include <aoclibs/mem/str.h>
#include <assert.h>
#include <stddef.h>

bool _mem_streq(const char *_Nonnull s1, const char *_Nonnull s2,
                const usize s1_len, const usize s2_len) {
        ASSERT(s1 != NULL && s2 != NULL, "%s",
               "passing NULL pointer to Nonnull parameter");
        ASSERT(s1_len > 0, "string length \"%d\" cannot be negative", s1_len);
        ASSERT(s2_len > 0, "string length \"%d\" cannot be negative", s2_len);

        if (s1_len != s2_len) return -1;

        for (usize i = 0; i < s1_len; i++) {
                if (s1[i] != s2[i]) return -1;
        }

        return 0;
}

bool _strreq(const char *_Nonnull s1, const char *_Nonnull s2,
             const usize s1_len, const usize s2_len) {
        ASSERT(s1 != NULL && s2 != NULL, "%s",
               "passing NULL pointer to Nonnull parameter");
        ASSERT(s1_len > 0, "string length \"%d\" cannot be negative", s1_len);
        ASSERT(s2_len > 0, "string length \"%d\" cannot be negative", s2_len);

        if (s1_len != s2_len) return -1;

        usize s1_pos = s1_len > 0 ? s1_len - 1 : 0;
        usize s2_pos = s2_len > 0 ? s2_len - 1 : 0;

        while (s1_pos != 0 && s2_pos != 0) {
                if (s1[s1_pos] != s2[s2_pos]) return -1;
                s1_pos--;
                s2_pos--;
        }

        return 0;
}
