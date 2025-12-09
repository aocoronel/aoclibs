#include <aoclibs/common.h>
#include <aoclibs/mem/str.h>
#include <assert.h>

Err _strcpy(char *s1, const char *s2, const usize s1_buff, usize s2_len) {
    assert(s1 != NULL && s2 != NULL);

    if (s2_len >= s1_buff) {
        return werr(-1, "Buffer is too small");
    }
    for (usize i = 0; i < s2_len; i++) {
        s1[i] = s2[i];
    }
    s1[s2_len] = '\0';
    return ok();
}
