#include <a_error_fn_err.h>
#include <a_error_fn_ok.h>
#include <a_error_types.h>
#include <a_mem_fn_strcat.h>
#include <assert.h>

Error _strcat(char *s1, const char *s2, usize s1_buff, usize s1_len, usize s2_len) {
    assert(s1 != NULL && s2 != NULL);

    if (s1_len + s2_len >= s1_buff) {
        return err(-1, "buffer is too small");
    }

    for (usize i = 0; i < s2_len; i++) {
        s1[s1_len + i] = s2[i];
    }
    s1[s1_len + s2_len] = '\0';

    return ok();
}
