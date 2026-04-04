#include "slices.h"

int main(int argc, char *argv[]) {
    Slice input = { .data = "\"Hello, world!\" $abc@abc!abc,. epic($alpha, maneuver(x));",
                    .len = strlen("Hello, world! $abc@abc!abc,. epic($alpha, maneuver(x));") };

    int remaining = input.len;

    Slice cursor = input;
    Slice out = { 0 };

    while (while_token(&cursor, &remaining, &out)) {
        printf("Token: '%.*s'\n", (int)out.len, out.data);
    }
    return 0;
}
