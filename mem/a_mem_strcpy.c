#include <a_error_fn_err.h>
#include <a_error_fn_ok.h>
#include <a_error_types.h>
#include <a_mem_fn_strcpy.h>
#include <assert.h>

Error _strcpy(char *s1, const char *s2, const usize s1_buff, usize s2_len) {
    assert(s1 != NULL && s2 != NULL);

    if (s2_len >= s1_buff) {
        return err(-1, "Buffer is too small");
    }
    for (usize i = 0; i < s2_len; i++) {
        s1[i] = s2[i];
    }
    s1[s2_len] = '\0';
    return ok();
}
