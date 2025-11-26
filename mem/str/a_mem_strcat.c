#include <a_error_fn_err.h>
#include <a_error_fn_ok.h>
#include <a_error_types.h>
#include <a_mem_fn_strcat.h>
#include <assert.h>
#include <stdio.h>

Error _strcat(char *_Nonnull s1, const char *_Nonnull s2,
              const usize s1_buff, const usize s1_len, const usize s2_len) {
        assert(s1 != NULL && s2 != NULL);
        if (s2_len + s1_len > s1_buff) return err(-1, NULL);
        for (i32 i = 0; i < s2_len; i++) {
                s1[s1_len + i] = s2[i];
        }
        s1[s1_buff - 1] = '\0';
        return ok();
}
