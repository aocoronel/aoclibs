#pragma once

#include "base.h"
#include "cstr.h"
#include "printfc.h"
#include "cstr.h"
#include "da.h"
#include "file.h"
#include "rc.h"

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

// akin to libc getdelim
size_t
read_by_delim(char **restrict buff, size_t *restrict size, const char delim, FILE *restrict fd) {
	$assert_nonnull(buff);
	$assert_nonnull(size);
	$assert_nonnull(fd);

	$catch(*buff == NULL || *size == 0) {
		*size = 128;
		*buff = (char *)malloc(*size);
		if (!*buff) return SIZE_MAX;
	}

	size_t pos = 0;

	for (;;) {
		int c = getc(fd);

		$catch(c == EOF) {
			if (pos == 0) return SIZE_MAX;
			break;
		}

		if (pos + 1 >= *size) {
			size_t new_size = *size * 2;
			char *new_ptr = (char *)realloc(*buff, new_size);
			if (!new_ptr) return SIZE_MAX;

			*buff = new_ptr;
			*size = new_size;
		}

		(*buff)[pos++] = (char)c;

		if (c == delim) break;
	}

	(*buff)[pos] = '\0';
	return pos;
}

const char *get_home_env() {
	static const char *home = NULL;
	if (!home) home = getenv("HOME");
	return home;
}

bool read_by_lines(Slice *out, char **restrict buff, size_t *restrict size, FILE *restrict fd) {
	char *p = NULL;
	size_t len = 0;

	len = read_by_delim(buff, size, '\n', fd);
	$catch(len == SIZE_MAX) return false;

	p = *buff;
	if ($likely(p[len - 1] == '\n')) {
		p[len - 1] = '\0';
	} else {
		// There was one project of mine, where I was making an experimental programming language
		// I had this problem, where the last newline couldn't be found, hopefully I had an assertion
		// to catch it
		$assert(p[len - 1] == '\0');
	}

	*out = (Slice){ .data = p, .len = len - 1 };
	return true;
}

int absolute_path_from(rc *output, Slice *path) {
	$assert_nonnull(path);
	$assert_nonnull(output);

	const char *home = get_home_env();
	$catch(!home) return 1;
	size_t home_len = strlen(home);

	char cwd[AOC_MAX_PATH] = { 0 };
	$catch(getcwd(cwd, sizeof(cwd)) == NULL) return 2;
	size_t cwd_len = strlen(cwd);

	const char *pos = path->data;
	size_t len = path->len;

	if (path->len > 2) {
		if (pos[0] == '~' && pos[1] == '/') {
			rc_cat(output, home, home_len);
			da_insert(output, '/');
			pos += 2;
			len -= 2;
		} else if (pos[0] == '.' && pos[1] == '/') {
			rc_cat(output, cwd, cwd_len);
			da_insert(output, '/');
			pos += 2;
			len -= 2;
		} else {
			rc_cat(output, cwd, cwd_len);
			da_insert(output, '/');
		}
	} else {
		if (pos[0] == '.' && pos[1] == '\0') {
			rc_cat(output, cwd, cwd_len);
			pos += 1;
			len -= 1;
		}
	}

	while (*pos) {
		if (*pos == '$') {
			pos++;

			char varname[256];
			size_t vi = 0;

			while ((*pos == '_' || isalnum((unsigned char)*pos)) && vi < sizeof(varname) - 1) {
				varname[vi++] = *pos++;
			}

			varname[vi] = '\0';

			const char *env = getenv(varname);
			$catch(!env) return 1;

			rc_cat(output, env, strlen(env));

			continue;
		}

		da_insert(output, *pos);
		pos++;
	}

	rc_cat(output, pos, len);

	return 0;
}

bool dir_walker(const char *path, Dir_Walker *dw) {
	$assert_nonnull(path != NULL);
	$assert_nonnull(dw);

	DIR *dir = opendir(path);
	$catch(!dir) return false;

	int8_t empty = 0;

	struct stat st;
	struct dirent *entry;
	char fullpath[AOC_MAX_PATH];

	while ((entry = readdir(dir)) != NULL) {
		$catch(cstr_eq(entry->d_name, ".") || cstr_eq(entry->d_name, "..")) continue;

		cstr_fmt_write(fullpath, AOC_MAX_PATH, "%s/%s", path, entry->d_name);

		File_Type ft = get_filetype(&st, fullpath);

		switch (ft) {
		case F_REG:
			if (dw->isreg != NULL) dw->isreg(ft, &st, fullpath);
			break;
		case F_DIR:
			if (dw->isdir != NULL) dw->isdir(ft, dw);
			break;
		case F_LNK:
			if (dw->islnk != NULL) dw->islnk(ft, &st, fullpath);
			break;
		case F_NULL:
			if (dw->isnull != NULL) dw->isnull(ft, &st, fullpath);
			break;
		default:
			break;
		}

		empty++;
	}

	if (empty == 0 && dw->isempty != NULL) dw->isempty(path);

	closedir(dir);
	return true;
}

File_Type get_filetype(struct stat *restrict st, const char *restrict path) {
	$assert_nonnull(path != NULL);

	if (lstat(path, st) == -1) return F_FAIL;

	if (S_ISREG(st->st_mode))
		return F_REG;
	else if (S_ISDIR(st->st_mode))
		return F_DIR;
	else if (S_ISLNK(st->st_mode))
		return F_LNK;

	return F_FAIL;
}

bool read_entire_file(rc *lines, const char *filepath) {
	$assert_nonnull(lines);

	FILE *fp = fopen(filepath, "r");
	$catch(!fp) return false;

	struct stat st;
	$catch(stat(filepath, &st) == -1) {
		fclose(fp);
		return false;
	}

	da_reserve(lines, (size_t)st.st_size);
	size_t n = fread(lines->data, sizeof(char), st.st_size, fp);
	lines->len = n;

	da_add_null(lines);

	fclose(fp);
	return true;
}

size_t dismantle_path(Slice **out, const char *path, size_t len) {
	$assert_nonnull(out);
	$assert_nonnull(path);

	struct slices_t {
		size_t len;
		size_t cap;
		Slice *data;
	} slices = { 0 };

	size_t i = 0;

	$catch(len == 0) {
		*out = NULL;
		return 0;
	}

	if (path[0] == '/') i = 1;

	while (i < len) {
		size_t j = index_of(path + i, '/', len - i);

		$catch(j == SIZE_MAX) {
			Slice s = { .data = path + i, .len = len - i };
			da_insert(&slices, s);
			break;
		}

		if (j == 0) {
			i += 1;
			continue;
		}

		Slice s = { .data = path + i, .len = j };
		da_insert(&slices, s);
		i += j + 1;
	}

	*out = slices.data;
	return slices.len;
}

char *make_path(
    char *restrict out, const size_t size, const Slice *restrict dirs, const size_t dir_count) {
	$assert_nonnull(out);
	$assert_nonnull(dirs);

	char *ptr = out;
	char *end = out + size;

	$range(0, dir_count, i) {
		const Slice *s = &dirs[i];
		size_t needed = s->len + 1; // '/'
		if ((size_t)(end - ptr) <= needed) return NULL;
		*ptr++ = '/';
		memcpy(ptr, s->data, s->len);
		ptr += s->len;
	}
	*ptr = '\0';

	return out;
}
