#define AOCLIBS_IMPLEMENTATION
#include "base/base.h"
#include "base/cstr.h"
#include "base/file.h"
#include <assert.h>
#include <stdio.h>

#define OUTPUT_FILE "aoclibs.h"
#define TEMPLATE_FILE "template.h"

FILE *output = NULL;
const char *file_to_open = NULL;

bool read_file(const char *ref file, bool ignore_include) {
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

                size_t idx = 0;
                if ((idx = aoc_cstr_has_at(buffer, size, "#include", STRLEN("#include"))) !=
                    SIZE_MAX) {
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
                                close = aoc_cstr_has_at(buffer + idx + 1, size - idx - 1, "\"", 1);
                        }
                        idx += 1;

                        if (close != 0) {
                                buffer[idx + close] = '\0';
                                if (!read_file(buffer + idx, true)) continue;
                                continue;
                        }
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

int main(void) {
        output = fopen(OUTPUT_FILE, "w");

        if (!output) {
                fprintf(stderr, "Failed to open file %s. %s\n", OUTPUT_FILE, strerror(errno));
                return false;
        }

        if (read_file(TEMPLATE_FILE, false) == false) return 1;

        fclose(output);

        return 0;
}
