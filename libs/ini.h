#ifndef AOCLIBS_INI_H_
#define AOCLIBS_INI_H_

#include "base.h"
#include "da.h"
#include "file.h"
#include "rc.h"
#include "arena.h"
#include "cstr.h"
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
        IniKey *data;
} IniKeys;

typedef struct {
        IniKeys keys;
        rc name;
} IniSection;

typedef struct {
        size_t len;
        size_t cap;
        IniSection *data;
} IniSections;

IniSections read_ini_file(Arena *arena, const char *file_path) {
        FILE *fp = fopen(file_path, "r");
        if (!fp) return (IniSections){};

        IniSections sections = { 0 };

        char *buffer = NULL;
        size_t size = 0;
        size_t new_line = 0;

        IniSection curr_section = { .keys = (IniKeys){ 0 } };

        aoc_arcl_cat(arena, &curr_section.name, "DEFAULT");
        aoc_dar_add_null(arena, &curr_section.name);

        aoc_dar_insert(arena, &sections, curr_section);

        for (; (new_line = read_by_delim(&buffer, &size, '\n', fp)) != SIZE_MAX;) {
                int open_brackets = 0;
                int close_brackets = 0;
                int equal = 0;

                int comment = 0;

                comment = aoc_index_of(buffer, ';', new_line);
                if (comment != SIZE_MAX) {
                        buffer[comment] = '\0';
                        new_line = comment;
                }
                comment = aoc_index_of(buffer, '#', new_line);
                if (comment != SIZE_MAX) {
                        buffer[comment] = '\0';
                        new_line = comment;
                }

                if ((open_brackets = aoc_index_of(buffer, '[', new_line)) != SIZE_MAX) {
                        close_brackets = aoc_index_of(buffer, ']', new_line);
                        if (close_brackets == SIZE_MAX) continue;

                        IniSection new_section = { .name = (rc){ 0 }, .keys = (IniKeys){ 0 } };
                        aoc_arc_cat(arena,
                                    &new_section.name,
                                    buffer + open_brackets + 1,
                                    close_brackets - 1);
                        aoc_dar_add_null(arena, &new_section.name);

                        aoc_dar_insert(arena, &sections, new_section);

                        curr_section = aoc_da_last(&sections);
                } else if ((equal = aoc_index_of(buffer, '=', new_line)) != SIZE_MAX) {
                        IniKeys *current_keys = &aoc_da_last(&sections).keys;

                        IniKey ini_key = { .key = (rc){ 0 }, .value = (rc){ 0 } };

                        CSlice key_slice = extract_between_whitespace(buffer, 0, equal - 1);

                        aoc_arc_cat(arena, &ini_key.key, key_slice.data, key_slice.len);
                        aoc_dar_add_null(arena, &ini_key.key);

                        CSlice value_slice = extract_between_whitespace(
                                buffer + equal + 1, 0, new_line - equal - 2);

                        aoc_arc_cat(arena, &ini_key.value, value_slice.data, value_slice.len);
                        aoc_dar_add_null(arena, &ini_key.value);

                        aoc_dar_insert(arena, current_keys, ini_key);
                }
        }

        free(buffer);
        return sections;
}

#endif
