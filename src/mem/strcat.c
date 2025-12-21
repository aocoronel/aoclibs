#include <aoclibs/common.h>
#include <aoclibs/mem/str.h>
#include <assert.h>

Err _mem_strcat(char *_Nonnull mut s1, const char *_Nonnull s2, usize s1_buff,
            usize s1_len, usize s2_len) {
        ASSERT(s1 != NULL && s2 != NULL, "%s",
               "passing NULL pointer to Nonnull parameter");
        ASSERT(s1_len > 0, "string length \"%d\" cannot be negative", s1_len);
        ASSERT(s2_len > 0, "string length \"%d\" cannot be negative", s2_len);

        if (s1_len + s2_len > s1_buff) return ErrBufferTooSmall;

        for (usize i = 0; i < s2_len; i++) {
                s1[s1_len + i] = s2[i];
        }
        s1[s1_len + s2_len] = '\0';

        return ok();
}
