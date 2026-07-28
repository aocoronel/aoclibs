#define AOC_IMPLEMENTATION
#include "include/base.h"
#include "include/cstr.h"
#include "include/file.h"
#include "include/fork.h"
#include "include/io.h"
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>

#define OUTPUT_FILE "aoclibs.h"
#define TEMPLATE_FILE "template.h"
#define TEMPLATE_FILE_LEN $strlen(TEMPLATE_FILE)

FILE *output = NULL;
const char *file_to_open = NULL;

bool disable_tunit = false;

#ifdef __cplusplus__
char *C_COMPILER = "g++";
#else
char *C_COMPILER = "gcc";
#endif

void read_source_files(File_Type ft, struct stat *st, const char *path);

bool read_file(const char *file, bool ignore_include) {
	FILE *fp = fopen(file, "r");
	if (!fp) {
		eprintf("error: file '%s' not found, skipping\n", file);
		return false;
	}

	char *buffer = NULL;
	size_t size = 0;
	size_t nread;

	int line_count = 0;

	while ((nread = read_by_delim(&buffer, &size, '\n', fp)) != SIZE_MAX) {
		line_count++;

#define MERGE_MATCH(s) cstr_has_at(buffer, size, s, $strlen(s))

		size_t pos = 0;
		if ((pos = MERGE_MATCH("#include")) != SIZE_MAX) {
			//        #include ...
			//        ^ pos
			assert(buffer[pos] == '#');

			pos += $strlen("#include");
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
		} else if (disable_tunit && MERGE_MATCH("// TEST_BEGIN") != SIZE_MAX) {
			while ((nread = read_by_delim(&buffer, &size, '\n', fp)) != SIZE_MAX) {
				line_count++;
				if (MERGE_MATCH("// TEST_END") != SIZE_MAX) {
					goto out;
				}
			}
out:
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

void read_source_files(File_Type ft, struct stat *st, const char *path) {
	if (!cstr_ends_with(path, strlen(path), ".c", 2)) return;

	if (!read_file(path, true)) return;

	return;
}

#define STRING                     \
	"#define AOC_IMPLEMENTATION\n" \
	"#define AOC_SPINNER\n"        \
	"#define CROWN\n"              \
	"#define LEXER\n"              \
	"#include \"aoclibs.h\"\n"

#define STRING_MAIN "#include \"aoclibs.h\"\n"

// I didn't want to deploy crown, so I made this silly flag parsing
#define flag(var, string)                     \
	$range(1, argc, i) {                      \
		if (cstr_eq(argv[i], "" string "")) { \
			var = true;                       \
			break;                            \
		}                                     \
	}

void usage() {
	eprintf(
	    "Usage: merge [OPTIONS]\n"
	    "  -h Displays this message and exits\n"
	    "  -no-test Disable TUnit tests and strip them from aoclibs.h\n");
}

int main(int argc, char *argv[]) {
	bool print_usage = false;

	flag(disable_tunit, "-no-test");
	flag(print_usage, "-h");

	if (print_usage) {
		usage();
		return 0;
	}

	output = fopen(OUTPUT_FILE, "w");

	if (!output) {
		eprintf("Failed to open file %s. %s\n", OUTPUT_FILE, strerror(errno));
		return 1;
	}

	fputs("#ifndef AOC_H\n", output);
	fputs("#define AOC_H\n", output);

	if (read_file(TEMPLATE_FILE, false) == false) return 1;

	fprintf(output, "%s", "#ifdef AOC_IMPLEMENTATION\n");
	dir_walk("src", .isreg = read_source_files);
	fprintf(output, "%s", "#endif // AOC_IMPLEMENTATION\n");
	fputs("#endif // AOC_H\n", output);

	fclose(output);

	eprintf("Generated ./aoclibs.h\n");

	if (!disable_tunit) {
		FILE *fp = fopen("test.c", "w");
		if (!fp) {
			perror("fopen(test.c, w)");
			return 1;
		}
		fwrite(STRING_MAIN, sizeof(char), $strlen(STRING_MAIN), fp);
		fclose(fp);
	}

	{
		FILE *fp = fopen("aoclibs.c", "w");
		if (!fp) {
			perror("fopen(aoclibs, w)");
			return 1;
		}
		fwrite(STRING, sizeof(char), $strlen(STRING), fp);
		fclose(fp);
	}

	if (!disable_tunit) {
#define OPTIMIZE "-O2", "-flto", "-fPIC"

		Fork_Options opt = { 0 };
		char *compile_args[] = { C_COMPILER,  "-std=c11",  "-c",  "-o",
			                     "aoclibs.o", "aoclibs.c", "-lm", NULL };
		opt.argv = compile_args;
		opt.err = true;
		Cmd_Result output = { 0 };
		int status = run_cmd(&output, opt);

		if (status != 0) {
			eprintf("%s", output.err.data);
			eprintf("Failed to build aoclibs.h. Got error: %d\n", status);
			return 1;
		} else {
			eprintf("Generated ./aoclibs.o\n");
		}
	}

	if (!disable_tunit) {
		Fork_Options opt = { 0 };
		// "-DTUNIT_SUBPROCESS",
		char *compile_args[] = { C_COMPILER, "-std=c11",  "-o",      "test", "test.c",
			                     "-lm",      "aoclibs.o", "-DTUNIT", NULL };
		opt.argv = compile_args;
		opt.err = true;
		Cmd_Result output = { 0 };
		int status = run_cmd(&output, opt);

		if (status != 0) {
			eprintf("%s", output.err.data);
			eprintf("Failed to build aoclibs.h. Got error: %d\n", status);
			return 1;
		}
	}

	if (!disable_tunit) {
		Fork_Options opt = { 0 };
		char *run_args[] = { "./test", NULL };
		opt.argv = run_args;
		opt.err = true;
		Cmd_Result output = { 0 };
		int status = run_cmd(&output, opt);

		if (disable_tunit) {
			eprintf("%s", output.err.data);
		} else {
			eprintf("Running tests:\n%s", output.err.data);
		}
		if (status != 0) {
			eprintf("Failed to run test. Got error %d\n", status);
			return 1;
		}
	}
	if (!disable_tunit) fputc('\n', stderr);

	return 0;
}
