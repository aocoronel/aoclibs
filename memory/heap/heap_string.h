#ifndef HEAP_STRING
#define HEAP_STRING

#include "heap_alloc.h"
#include <stddef.h>

/*
 * Define heap allocated strings.
 * Enforces known length and memory size.
*/
typedef struct {
        HeapPtr heap; // heap_alloc.h:9
        size_t length; // current length (not counting '\0')
} String;

String *string_create(void);

String *string_from(const char *src);

void string_free(String *s);

String string_init(void) {

int string_reserve(String *s, size_t new_cap);

int string_append(String *s, const char *suffix);

#endif // !HEAP_STRING

// int main(void) {
//         String *s = string_from("Hello");
//         string_append(s, ", world!");
//         printf("%s\n", (char *)s->heap.ptr); // prints: Hello, world!
//         string_free(s);
//         return 0;
// }
