#include <aoclibs/common.h>
#include <aoclibs/mem/str.h>
#include <stddef.h>

ei32 _mem_strfind(const char *_Nonnull s, const char c, const usize s_len) {
        ASSERT(s != NULL, "%s", "passing NULL pointer to Nonnull parameter");
        ASSERT(s_len > 0, "string length \"%d\" cannot be negative", s_len);

        for (usize i = 0; i < s_len; i++) {
                if (s[i] == c) return werr_i32(ok(), i);
        }

        return werr_i32(ErrCharNotFound, 0);
}

ei32 _mem_strrfind(const char *_Nonnull s, const char c, const usize s_len) {
        ASSERT(s != NULL, "%s", "passing NULL pointer to Nonnull parameter");
        ASSERT(s_len > 0, "string length \"%d\" cannot be negative", s_len);

        for (usize i = s_len; i > 0; i--) {
                if (s[i] == c) return werr_i32(ok(), i);
        }

        return werr_i32(ErrCharNotFound, 0);
}
