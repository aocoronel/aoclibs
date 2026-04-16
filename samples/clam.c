#include "aoclibs.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    rc lines = { 0 };
    if (!read_entire_file(&lines, "clam.clam")) {
        return 1;
    }
    // Slice cursor = { .data = lines.data, .len = lines.len };
    Slice out = { 0 };

    const char *p = lines.data;
    size_t begin = 0;
    size_t end = lines.len;
    Slice cursor = { 0 };

    while (begin < end) {
        cursor = while_extract_next_word(p, &begin, end);
        int len = cursor.len;

        if (cursor.len == 0) break;

        // printf("Processing word: %.*s\n", (int)cursor.len, cursor.data);
        while (while_token(&cursor, &len, &out)) {
            printf("%.*s", (int)out.len, out.data);
        }

        if (out.data[out.len] != '\n') {
            fputc(' ', stdout);
            // printf("add space here\n");
        } else {
            fputc('\n', stdout);
        }
    }
    return 0;
}
