#include <aoclibs/common.h>
#include <aoclibs/mem/str.h>
#include <assert.h>

Err _mem_strcpy(char *_Nonnull mut s1, const char *_Nonnull s2, const usize s1_buff,
            usize s2_len) {
        ASSERT(s1 != NULL && s2 != NULL, "%s",
               "passing NULL pointer to Nonnull parameter");
        ASSERT(s2_len > 0, "string length \"%d\" cannot be negative", s2_len);

        if (s2_len >= s1_buff) return ErrBufferTooSmall;

        for (usize i = 0; i < s2_len; i++) {
                s1[i] = s2[i];
        }

        s1[s2_len] = '\0';
        return ok();
}
