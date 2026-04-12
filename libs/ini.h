#ifndef AOCLIBS_INI_H_
#define AOCLIBS_INI_H_

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

#endif
