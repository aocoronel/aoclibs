#define AOCLIBS_IMPLEMENTATION
#include "include/base.h"
#include "include/cstr.h"
#include "include/file.h"
#include "include/fork.h"
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>

#define OUTPUT_FILE "aoclibs.h"
#define TEMPLATE_FILE "template.h"
#define TEMPLATE_FILE_LEN STRLEN(TEMPLATE_FILE)

FILE *output = NULL;
const char *file_to_open = NULL;

void read_source_files(const char *path);

bool read_file(const char *file, bool ignore_include) {
    FILE *fp = fopen(file, "r");
    if (!fp) {
        fprintf(stderr, "File not found: %s\n", file);
        return false;
    }

    char *buffer = NULL;
    size_t size = 0;
    size_t nread;

    int line_count = 0;

    while ((nread = read_by_delim(&buffer, &size, '\n', fp)) != SIZE_MAX) {
        line_count++;

#define MERGE_MATCH(s) cstr_has_at(buffer, size, s, STRLEN(s))

        size_t idx = 0;
        if ((idx = MERGE_MATCH("#include")) != SIZE_MAX) {
            //        #include ...
            //        ^ idx
            assert(buffer[idx] == '#');

            idx += STRLEN("#include");
            for (size_t i = idx; i < size && buffer[i] == ' '; i++) {
                idx++;
            }

            if (ignore_include) {
                if (buffer[idx] == '<')
                    goto print;
                else if (buffer[idx] == '"')
                    continue;
            }

            int close = 0;
            if (buffer[idx] == '"') {
                close = cstr_has_at(buffer + idx + 1, size - idx - 1, "\"", 1);
            }
            idx += 1;

            if (close != 0) {
                buffer[idx + close] = '\0';
                if (!read_file(buffer + idx, true)) continue;
                continue;
            }
            continue;
        } else if (memcmp(file, TEMPLATE_FILE, TEMPLATE_FILE_LEN) == 0 &&
                   (idx = MERGE_MATCH("#ifdef AOCLIBS_IMPLEMENTATION")) != SIZE_MAX) {
            fprintf(output, "%s", buffer);
            dir_walk("src", true, NULL, read_source_files, NULL, NULL, NULL);
            continue;
        }
print:
        fprintf(output, "%s", buffer);
    }

    assert(buffer != NULL);
    free(buffer);
    assert(fp != NULL);
    fclose(fp);
    return true;
}

void read_source_files(const char *path) {
    if (!cstr_ends_with(path, strlen(path), ".c", 2)) return;
    if (!read_file(path, true)) return;
    return;
}

#define STRING                             \
    "#define AOCLIBS_IMPLEMENTATION\n"     \
    "#include <string.h>\n"                \
    "#include <stdlib.h>\n"                \
    "#include \"aoclibs.h\"\n"             \
    "int main(int argc, char *argv[]) {\n" \
    "        return 0;\n"                  \
    "}\n"

int main(void) {
    output = fopen(OUTPUT_FILE, "w");

    if (!output) {
        fprintf(stderr, "Failed to open file %s. %s\n", OUTPUT_FILE, strerror(errno));
        return false;
    }

    if (read_file(TEMPLATE_FILE, false) == false) return 1;

    fclose(output);

    FILE *fp = fopen("test.c", "w");
    fwrite(STRING, sizeof(char), STRLEN(STRING), fp);
    fclose(fp);

    char *compile_args[] = { "gcc", "-o", "test", "test.c", "-lm", NULL };
    PipeResult result = run_cmd(compile_args, NULL);

    if (result.status != 0) {
        fprintf(stderr, "Failed to build aoclibs.h. Got error: %d\n", result.status);
    }

    char *run_args[] = { "./test", NULL };
    result = run_cmd(run_args, NULL);

    if (result.status != 0) {
        fprintf(stderr, "Failed to run test. Got error %d\n", result.status);
    }

    printf("Created ./aoclibs.h\n");

    return 0;
}
