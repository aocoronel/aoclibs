#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "heap_alloc.h"

typedef struct {
        HeapPtr heap; // Use HeapPtr for memory management
        size_t length; // current length (not counting '\0')
} String;

String *string_create(void) {
        String *s = malloc(sizeof(String));
        if (!s) return NULL;
        heap_init(&s->heap);
        if (!heap_alloc(&s->heap, 16)) {
                free(s);
                return NULL;
        }
        s->length = 0;
        ((char *)s->heap.ptr)[0] = '\0';
        return s;
}

String *string_from(const char *src) {
        String *s = malloc(sizeof(String));
        if (!s) return NULL;
        size_t len = strlen(src);
        heap_init(&s->heap);
        if (!heap_alloc(&s->heap, len + 1)) {
                free(s);
                return NULL;
        }
        memcpy(s->heap.ptr, src, len + 1);
        s->length = len;
        return s;
}

void string_free(String *s) {
        if (!s) return;
        heap_free(&s->heap);
        free(s);
}

int string_reserve(String *s, size_t new_cap) {
        if (new_cap <= s->heap.mem) return 0;
        if (!resize_alloc(&s->heap, new_cap)) return -1;
        return 0;
}

String string_init(void) {
        HeapPtr h;
        heap_init(&h);
        String s = {
                .heap = h,
                .length = 0,
        };
        return s;
}

int string_append(String *s, const char *suffix) {
        size_t add_len = strlen(suffix);
        size_t needed = s->length + add_len + 1;
        if (needed > s->heap.mem) {
                if (string_reserve(s, needed * 2) != 0) return -1;
        }
        memcpy((char *)s->heap.ptr + s->length, suffix, add_len + 1);
        s->length += add_len;
        return 0;
}
