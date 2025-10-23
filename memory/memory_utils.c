#include <stddef.h>
#include "heap_modify.h"

void *memory_copy(void *dest, const void *src, size_t n) {
        char *d = (char *)dest;
        const char *s = (const char *)src;
        for (size_t i = 0; i < n; i++) {
                d[i] = s[i];
        }
        return dest;
}

void *memory_set(void *s, int c, size_t n) {
        unsigned char *p = (unsigned char *)s;
        for (size_t i = 0; i < n; i++) {
                p[i] = (unsigned char)c;
        }
        return s;
}

void *memory_move(void *dest, const void *src, size_t n) {
        char *d = (char *)dest;
        const char *s = (const char *)src;
        if (d < s) {
                // Copy forward
                for (size_t i = 0; i < n; i++) {
                        d[i] = s[i];
                }
        } else if (d > s) {
                // Copy backward
                for (size_t i = n; i > 0; i--) {
                        d[i - 1] = s[i - 1];
                }
        }
        return dest;
}
