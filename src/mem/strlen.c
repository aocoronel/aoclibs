#include <aoclibs/common.h>
#include <aoclibs/mem/str.h>
#include <assert.h>
#include <stddef.h>

usize _strlen(const char *s, const usize buff) {
        assert(s != NULL);
        for (i32 i = 0; i < buff - 1; i++) {
                if (s[i] == '\0') return i;
        }
        return buff;
}
