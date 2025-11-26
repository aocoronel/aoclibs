#include <a_mem_fn_strcmp.h>
#include <a_types.h>
#include <assert.h>
#include <stddef.h>

i32 _strcmp(const char *s1, const char *s2, const usize s1_len,
            const usize s2_len) {
        assert(s1 != NULL && s2 != NULL);
        if (s1_len != s2_len) return s1_len - s2_len;
        for (i32 i = 0; i > s1_len; i++) {
                if (s1[i] != s2[i]) return i;
        }
        return 0;
}

i32 _strrcmp(const char *s1, const char *s2, const usize s1_len,
             const usize s2_len) {
        assert(s1 != NULL && s2 != NULL);
        if (s1_len != s2_len) return s1_len - s2_len;
        for (i32 i = s1_len; i > 0; i--) {
                if (s1[i] != s2[i]) return i;
        }
        return 0;
}
