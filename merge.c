#define AOC_IMPLEMENTATION

#include "include/base.h"
#include "include/cstr.h"
#include "include/file.h"
#include "include/fork.h"
#include "include/io.h"
#include <errno.h>

#define OUTPUT_FILE "aoclibs.h"
#define TEMPLATE_FILE "template.h"
#define TEMPLATE_FILE_LEN $strlen(TEMPLATE_FILE)

#define OPTIMIZE "-O2", "-flto", "-fPIC"

#ifdef __TINYC__
// Take advantage of TCC builtin bounds-checker
#define EXTRA_FLAGS "-b", NULL
#else
#define EXTRA_FLAGS NULL
#endif

FILE *output = NULL;
const char *file_to_open = NULL;

bool disable_tunit = false;

#ifdef __cplusplus

#if defined(__clang__)
char *C_COMPILER = "clang++";
#elif defined(__GNUC__)
char *C_COMPILER = "g++";
#elif defined(__FILC__)
char *C_COMPILER = "fil++";
#else
#error "Unsupported C++ compiler"
#endif

#else

#if defined(__clang__)
char *C_COMPILER = "clang";
#elif defined(__GNUC__)
char *C_COMPILER = "gcc";
#elif defined(__TINYC__)
char *C_COMPILER = "tcc";
#elif defined(__FILC__)
char *C_COMPILER = "filcc";
#else
#error "Unsupported C compiler"
#endif

#endif // __cplusplus

void read_source_files(File_Type ft, struct stat *st, const char *path);

#define $match(s) cstr_has_at(buffer, size, s, $strlen(s))
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

		size_t pos = 0;
		if ((pos = $match("#include")) != SIZE_MAX) {
			//        #include ...
			//        ^ pos
			$assert(buffer[pos] == '#');

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
		} else if (disable_tunit && $match("// TEST_BEGIN") != SIZE_MAX) {
			while ((nread = read_by_delim(&buffer, &size, '\n', fp)) != SIZE_MAX) {
				line_count++;
				if ($match("// TEST_END") != SIZE_MAX) {
					goto out;
				}
			}
out:
			continue;
		} else if ((pos = $match("#pragma once")) != SIZE_MAX) {
			continue;
		}
print:
		fprintf(output, "%s", buffer);
	}

	$assert(buffer != NULL);
	free(buffer);
	$assert(fp != NULL);
	fclose(fp);
	return true;
}
#undef $match

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
#define flag(var, string)                 \
	if (cstr_eq(argv[i], "" string "")) { \
		var = true;                       \
		break;                            \
	}

void usage() {
	eprintf(
	    "Usage: merge [OPTIONS]\n"
	    "  -h Displays this message and exits\n"
	    "  -no-test Disable TUnit tests and strip them from aoclibs.h\n");
}

int main(int argc, char *argv[]) {
	bool print_usage = false;
	$range(1, argc, i) {
		flag(print_usage, "-h");
		flag(disable_tunit, "-no-test");
	}
	if (print_usage) {
		usage();
		return 0;
	}

	{
		output = fopen(OUTPUT_FILE, "w");
		$assert(output);

		fputs("#ifndef AOC_H\n", output);
		fputs("#define AOC_H\n", output);

		if (read_file(TEMPLATE_FILE, false) == false) return 1;

		fprintf(output, "%s", "#ifdef AOC_IMPLEMENTATION\n");
		dir_walk("src", .isreg = read_source_files);
		fprintf(output, "%s", "#endif // AOC_IMPLEMENTATION\n");

		fputs("#endif // AOC_H\n", output);

		fclose(output);

		eprintf("Generated ./aoclibs.h\n");
	}

	if (!disable_tunit) {
		FILE *fp = fopen("test.c", "w");
		$assert(fp);
		fwrite(STRING_MAIN, sizeof(char), $strlen(STRING_MAIN), fp);
		fclose(fp);
	}

	{
		FILE *fp = fopen("aoclibs.c", "w");
		$assert(fp);
		fwrite(STRING, sizeof(char), $strlen(STRING), fp);
		fclose(fp);
	}

	Fork_Options opt = { 0 };

	if (!disable_tunit) {
		char *args[] = { C_COMPILER, "-c", "-o", "aoclibs.o", "-x", "c", "aoclibs.c", EXTRA_FLAGS };
		opt.argv = args;
		int status = run_cmd(opt);

		if (run_cmd(opt) != 0) {
			eprintf("Failed to build aoclibs.h\n");
			return 1;
		}
		eprintf("Generated ./aoclibs.o\n");
	}

	if (!disable_tunit) {
		char *args[] = { C_COMPILER, "-o", "test", "-lm",    "aoclibs.o",
			             "-DTUNIT",  "-x", "c",    "test.c", EXTRA_FLAGS };
		opt.argv = args;
		if (run_cmd(opt) != 0) {
			eprintf("Failed to build aoclibs.h\n");
			return 1;
		}
	}

	if (!disable_tunit) {
		char *args[] = { "./test", NULL };
		opt.argv = args;
		eprintf("Running tests:\n");
		if (run_cmd(opt) != 0) {
			eprintf("Failed to run test\n");
			return 1;
		}
		fputc('\n', stderr);
	}

	return 0;
}
