#ifndef AOC_INI_H_
#define AOC_INI_H_

#include "rc.h"
#include <ctype.h>
#include "cstr.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef struct {
	const char *key;
	const char *value;

	rc key_string;
	rc value_string;
} IniKey;

typedef struct {
	FILE *fd;
	char *pos;
	size_t size;
} IniIterator;

typedef struct {
	const char *message;
	size_t line;
	size_t column;

	const char *pos;
	size_t len;
} IniError;

typedef struct {
	IniIterator iterator;
	IniError error;
	rc section;

	Slice key;
	Slice value;
} Ini;

#ifdef DEBUG
#define print_loc eprintf("%s:%d location: %s\n", __FILE__, __LINE__, __func__);
#else
#define print_loc
#endif

AOCDEF bool ini_init(Ini *restrict output, FILE *restrict fp);
AOCDEF bool ini_next_line(Ini *ini);
AOCDEF bool ini_has_error(Ini *ini);
AOCDEF bool ini_parse(Ini *ini);
AOCDEF void ini_print_error(Ini *restrict ini, const char *restrict file);
AOCDEF void ini_deinit(Ini *ini);

#ifdef AOC_INI
#include "da.h"
#include "file.h"
#include "printfc.h"
#include "rc.h"
#include "cstr.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// fp should be opened to read and closed by the caller.
bool ini_init(Ini *output, FILE *fp) {
	$assert_nonnull(output);
	$assert_nonnull(fp);

	*output = (Ini) {
        .iterator = {
            .pos = NULL,
            .size = 0,
            .fd = fp,
        },
        .error = { 0 },
        .section = { 0 },
    };

	output->section.len = $strlen("DEFAULT");
	da_add(&output->section, "DEFAULT", $strlen("DEFAULT"), 0);
	da_add_null(&output->section);

	return true;
}

#ifdef DEBUG
#define print_loc eprintf("%s:%d location: %s\n", __FILE__, __LINE__, __func__);
#else
#define print_loc
#endif

bool ini_next_line(Ini *ini) {
	$assert_nonnull(ini);
	IniIterator *it = &ini->iterator;
	IniError *error = &ini->error;

	while (true) {
		size_t newline = read_by_delim(&it->pos, &it->size, '\n', it->fd);
		if (newline == SIZE_MAX) return false;

		error->line++;

		if (newline == 0) continue;

		if (it->pos[0] == ';' || it->pos[0] == '#') continue;

		error->column = 1;
		error->pos = it->pos;
		error->len = newline;

		return true;
	}
}

bool ini_has_error(Ini *ini) {
	$assert_nonnull(ini);
	if (ini->error.message) {
		return true;
	} else {
		return false;
	}
}

bool ini_parse(Ini *ini) {
	$assert_nonnull(ini);
	if (!ini_next_line(ini)) return false;

	IniIterator *it = &ini->iterator;
	IniError *error = &ini->error;

	size_t new_line = error->len;

	error->message = NULL;

	const char *pos = it->pos;

	if (*pos == '[') {
		// skip '['
		{
			pos++;
			new_line--;
		}

		const char *close_bracket = memchr(pos, ']', new_line);
		if (!close_bracket) {
			error->message = "could not find ending ']'";
			return false;
		}

		Slice section = { .data = pos, .len = (size_t)(close_bracket - (pos)) };
		slice_trim(&section);

		ini->section.len = section.len;
		da_add(&ini->section, section.data, section.len, 0);
		da_add_null(&ini->section);

		return ini_parse(ini);
	} else {
		bool key_is_array = false;
		const char *equal = memchr(pos, '=', new_line);
		if (!equal) {
			error->message = "expected 'key=value' pair";
			return false;
		}

		if ((equal - pos) == new_line) {
			error->message = "value not set";
			return false;
		}

		{
			Slice key = { .data = pos, .len = (size_t)(equal - pos) };
			slice_trim(&key);
			ini->key = key;
		}

		{
			Slice value = { .data = equal + 1, .len = (size_t)((pos + new_line) - (equal + 1)) };
			slice_trim(&value);

			value = slice_extract_from_substring(&value);

			slice_chop_right_by(&value, ';');
			slice_chop_right_by(&value, '#');

			ini->value = value;
		}

		return true;
	}
}

void ini_print_error(Ini *ini, const char *file) {
	$assert_nonnull(ini);
	$assert_nonnull(file);
	fprintf(stderr,
			"%s:%zu:%zu: error: %s\n",
			file,
			ini->error.line,
			ini->error.column,
			ini->error.message);
}

void ini_deinit(Ini *ini) {
	$assert_nonnull(ini);
	free(ini->iterator.pos);
	da_free(&ini->section);
}
#endif

// int main() {
//     print_loc;
//     FILE *fp = fopen("test.ini", "r");
//     if (!fp) return 1;
//
//     Ini ini = { 0 };
//     ini_init(&ini, fp);
//     while (ini_parse(&ini)) {
//         printf("Section: %.*s\n", (int)ini.section.len, ini.section.data);
//         printf("  key = %.*s\n", (int)ini.key.len, ini.key.data);
//         printf("  value = %.*s\n", (int)ini.value.len, ini.value.data);
//     }
//
//     if (ini_has_error(&ini)) {
//         ini_print_error(&ini, "test.ini");
//     }
//
//     ini_deinit(&ini);
//
//     $assert(fp);
//     fclose(fp);
//
//     return 0;
// }

#endif
