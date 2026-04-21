#ifndef AOCLIBS_INI_H_
#define AOCLIBS_INI_H_

#include "arena.h"
#include "rc.h"
#include "slices.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    rc key;
    rc value;
} IniKey;

typedef struct {
    size_t len;
    size_t cap;
    IniKey *null data;
} IniKeys;

typedef struct {
    IniKeys keys;
    rc name;
} IniSection;

typedef struct {
    size_t len;
    size_t cap;
    IniSection *null data;
} IniSections;

AOCDEF void ini_insert_key(Arena *a, IniKeys *keys, const Slice key, const Slice value);
AOCDEF void ini_insert_section(Arena *a, IniSections *section, const Slice name);
AOCDEF IniSections ini_read_fd(Arena *arena, FILE *fd);
AOCDEF IniSections ini_read(Arena *arena, const char *file_path);
AOCDEF int ini_write_fd(IniSections sections, FILE *fd);
AOCDEF int ini_write(IniSections sections, const char *file_path);

#ifdef AOCLIBS_INI
#include "arena.h"
#include "base.h"
#include "cstr.h"
#include "da.h"
#include "file.h"
#include "slices.h"
#include "rc.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

void ini_insert_key(Arena *a, IniKeys *keys, const Slice key, const Slice value) {
    IniKey k = { 0 };
    arc_cat(a, &k.key, key.data, key.len);
    dar_add_null(a, &k.key);

    arc_cat(a, &k.value, value.data, value.len);
    dar_add_null(a, &k.value);

    dar_insert(a, keys, k);
}

void ini_insert_section(Arena *a, IniSections *section, const Slice name) {
    IniSection s = { 0 };
    arc_cat(a, &s.name, name.data, name.len);
    dar_add_null(a, &s.name);

    dar_insert(a, section, s);
}

IniSections ini_read_fd(Arena *arena, FILE *fd) {
    if (!fd) return (IniSections){};

    IniSections sections = { 0 };

    char *buffer = NULL;
    size_t size = 0;
    size_t new_line = 0;

    Slice default_section = slice("DEFAULT");
    ini_insert_section(arena, &sections, default_section);

    IniSection curr_section = da_last(&sections);

    for (; (new_line = read_by_delim(&buffer, &size, '\n', fd)) != SIZE_MAX;) {
        int open_brackets = 0;
        int close_brackets = 0;
        int equal = 0;

        int comment = 0;

        comment = index_of(buffer, ';', new_line);
        if (comment != SIZE_MAX) {
            buffer[comment] = '\0';
            new_line = comment;
        }
        comment = index_of(buffer, '#', new_line);
        if (comment != SIZE_MAX) {
            buffer[comment] = '\0';
            new_line = comment;
        }

        // Declaration
        if ((equal = index_of(buffer, '=', new_line)) != SIZE_MAX) {
            IniKeys *current_keys = &da_last(&sections).keys;

            size_t begin = 0;
            Slice key_slice = while_next_word(buffer, &begin, equal);
            begin = equal + 1;

            // Array
            if (slice_ends_with(&key_slice, "[]", 2)) {
            }

            Slice value_slice = while_next_word(buffer, &begin, new_line);

            ini_insert_key(arena, current_keys, key_slice, value_slice);
        }
        // New Section
        else if ((open_brackets = index_of(buffer, '[', new_line)) != SIZE_MAX) {
            close_brackets = index_of(buffer, ']', new_line);
            if (close_brackets == SIZE_MAX) continue;

            // Subsection
            if (cstr_begins_with(buffer, close_brackets - 1, "[.", 2)) {
            }

            IniSection new_section = { .name = (rc){ 0 }, .keys = (IniKeys){ 0 } };

            Slice s_name = { .data = buffer + open_brackets + 1, .len = close_brackets - 1 };
            ini_insert_section(arena, &sections, s_name);

            curr_section = da_last(&sections);
        }
    }

    free(buffer);
    return sections;
}

IniSections ini_read(Arena *arena, const char *file_path) {
    ASSERT_NONNULL(arena != NULL);
    ASSERT_NONNULL(file_path != NULL);
    FILE *fd = fopen(file_path, "r");
    if (!fd) {
        return (IniSections){};
    }
    IniSections s = ini_read_fd(arena, fd);
    fclose(fd);
    return s;
}

int ini_write_fd(IniSections sections, FILE *fd) {
    foreach (&sections, i) {
        IniKeys keys = sections.data[i].keys;
        fprintf(fd, "[%s]\n", sections.data[i].name.data);
        foreach (&keys, j) {
            fprintf(fd, "    %s = %s\n", keys.data[j].key.data, keys.data[j].value.data);
        }
    }
    return 0;
}

int ini_write(IniSections sections, const char *file_path) {
    FILE *fd = fopen(file_path, "w");
    int err = ini_write_fd(sections, fd);
    fclose(fd);
    return 0;
}
#endif

#endif
