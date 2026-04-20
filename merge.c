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

void read_source_files(const FileMetadata *data);

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

        size_t pos = 0;
        if ((pos = MERGE_MATCH("#include")) != SIZE_MAX) {
            //        #include ...
            //        ^ pos
            assert(buffer[pos] == '#');

            pos += STRLEN("#include");
            for (size_t i = pos; i < size && buffer[i] == ' '; i++) {
                pos++;
            }

            if (ignore_include) {
                if (buffer[pos] == '<')
                    goto print;
                else if (buffer[pos] == '"')
                    continue;
            }

            int close = 0;
            if (buffer[pos] == '"') {
                close = cstr_has_at(buffer + pos + 1, size - pos - 1, "\"", 1);
            }
            pos += 1;

            if (close != 0) {
                buffer[pos + close] = '\0';
                if (!read_file(buffer + pos, true)) continue;
                continue;
            }
            continue;
        } else if ((pos = MERGE_MATCH("#pragma once")) != SIZE_MAX) {
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

void read_source_files(const FileMetadata *data) {
    if (!cstr_ends_with(data->name, strlen(data->name), ".c", 2)) return;

    fprintf(output, "%s", "#ifdef AOCLIBS_IMPLEMENTATION\n");
    if (!read_file(data->name, true)) return;
    fprintf(output, "%s", "#endif // AOCLIBS_IMPLEMENTATION\n");

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
    dir_walk("src", .isreg = read_source_files);

    fclose(output);

    FILE *fp = fopen("test.c", "w");
    fwrite(STRING, sizeof(char), STRLEN(STRING), fp);
    fclose(fp);

    char *compile_args[] = { "gcc", "-o", "test", "test.c", "-lm", NULL };
    PipeResult result = run_cmd(compile_args, NULL);

    if (result.status != 0) {
        fprintf(stderr, "Failed to build aoclibs.h. Got error: %d\n", result.status);
        return 1;
    }

    char *run_args[] = { "./test", NULL };
    result = run_cmd(run_args, NULL);

    if (result.status != 0) {
        fprintf(stderr, "Failed to run test. Got error %d\n", result.status);
        return 1;
    }

    printf("Created ./aoclibs.h\n");

    return 0;
}
