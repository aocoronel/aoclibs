#include <a_error_fn_err.h>
#include <a_error_fn_ok.h>
#include <a_error_types.h>
#include <a_mem_fn_strcpy.h>
#include <assert.h>
#include <stdio.h>

Error _strcpy(char *s1, const char *s2, const usize s1_buff,
              const usize s2_buff, usize s2_len) {
        assert(s2 != NULL);
        if (s2_buff > s1_buff) return err(-1, NULL);
        for (i32 i = 0; i < s2_len; i++) {
                s1[i] = s2[i];
        }
        s1[s1_buff - 1] = '\0';
        return ok();
}
