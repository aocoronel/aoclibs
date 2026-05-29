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

void read_source_files(FileType ft, struct stat *st, const char *path);

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

void read_source_files(FileType ft, struct stat *st, const char *path) {
    if (!cstr_ends_with(path, strlen(path), ".c", 2)) return;

    if (!read_file(path, true)) return;

    return;
}

#define STRING                         \
    "#define AOCLIBS_IMPLEMENTATION\n" \
    "#include <string.h>\n"            \
    "#include <stdlib.h>\n"            \
    "#include \"aoclibs.h\"\n"

#define STRING_MAIN                        \
    "#define AOCLIBS_IMPLEMENTATION\n"     \
    "#include <string.h>\n"                \
    "#include <stdlib.h>\n"                \
    "#include \"aoclibs.h\"\n"             \
    "int main(int argc, char *argv[]) {\n" \
    "        return 0;\n"                  \
    "}\n"

// I didn't want to deploy crown, so I made this silly flag parsing
#define flag(var, string)                     \
    range(1, argc, i) {                       \
        if (cstr_eq(argv[i], "" string "")) { \
            var = true;                       \
            break;                            \
        }                                     \
    }

// TODO: add option to strip tests from header files
int main(int argc, char *argv[]) {
    bool disable_tunit = false;
    bool compile_object = false;

    flag(disable_tunit, "-no-test");
    flag(compile_object, "-obj");

    output = fopen(OUTPUT_FILE, "w");

    if (!output) {
        fprintf(stderr, "Failed to open file %s. %s\n", OUTPUT_FILE, strerror(errno));
        return false;
    }

    fputs("#ifndef AOCLIBS_H\n", output);
    fputs("#define AOCLIBS_H\n", output);

    if (read_file(TEMPLATE_FILE, false) == false) return 1;

    fprintf(output, "%s", "#ifdef AOCLIBS_IMPLEMENTATION\n");
    dir_walk("src", .isreg = read_source_files);
    fprintf(output, "%s", "#endif // AOCLIBS_IMPLEMENTATION\n");
    fputs("#endif // AOCLIBS_H\n", output);

    fclose(output);

    {
        FILE *fp = fopen("test.c", "w");
        if (!fp) {
            perror("fopen(test.c, w)");
            return 1;
        }
        disable_tunit ? fwrite(STRING_MAIN, sizeof(char), STRLEN(STRING_MAIN), fp) :
                        fwrite(STRING, sizeof(char), STRLEN(STRING), fp);

        fclose(fp);
    }

    if (compile_object) {
        FILE *fp = fopen("aoclibs.c", "w");
        if (!fp) {
            perror("fopen(aoclibs, w)");
            return 1;
        }
        fwrite(STRING, sizeof(char), STRLEN(STRING), fp);
        fclose(fp);
    }

    {
        char *compile_args[] = { "gcc",
                                 "-o",
                                 "test",
                                 "test.c",
                                 "-lm",
                                 // "-DTUNIT_SUBPROCESS",
                                 "-DHEAP_TRACE",
                                 disable_tunit ? NULL : "-DTUNIT",
                                 NULL };
        CmdResult output = { 0 };
        int status = run_cmd(compile_args, NULL, &output, .in = false, .out = false, .err = true);

        if (status != 0) {
            eprintf("%s", output.err.data);
            eprintf("Failed to build aoclibs.h. Got error: %d\n", status);
            return 1;
        }
    }

    char *run_args[] = { "./test", NULL };

    {
        CmdResult output = { 0 };
        int status = run_cmd(run_args, NULL, &output, .in = false, .out = false, .err = true);

        if (disable_tunit) {
            eprintf("%s", output.err.data);
        } else {
            eprintf("Running tests:\n%s", output.err.data);
        }
        if (status != 0) {
            fprintf(stderr, "Failed to run test. Got error %d\n", status);
            return 1;
        }
    }
    if (!disable_tunit) fputc('\n', stderr);

    if (compile_object) {
        char *compile_args[] = { "gcc", "-c",        "-O2",       "-flto", "-pipe", "-fPIC",
                                 "-o",  "aoclibs.o", "aoclibs.c", "-lm",   NULL };
        CmdResult output = { 0 };
        int status = run_cmd(compile_args, NULL, &output, .in = false, .out = false, .err = true);

        if (status != 0) {
            eprintf("%s", output.err.data);
            eprintf("Failed to build aoclibs.h. Got error: %d\n", status);
            return 1;
        } else {
            eprintf("Generated aoclibs.o\n");
        }
    }

    eprintf("Generated ./aoclibs.h\n");

    return 0;
}
