#include "arena.h"
#include "da.h"
#include <stdio.h>
#include "ini.h"

int main(int argc, char **argv) {
        const char *file_path = "basic.ini";

        FILE *fp = fopen(file_path, "w");
        fprintf(fp, "cwd = .\nuser = aoc\n[PATHS]\ndir = ~/\nrecurse = true\nepic = true\n");
        fclose(fp);

        Arena IniArena = { 0 };
        IniSections sections = ini_read(&IniArena, file_path);

        Slice test_section = aoc_slice("Testing");
        Slice test_key = aoc_slice("key");
        Slice test_value = aoc_slice("value");

        ini_insert_section(&IniArena, &sections, test_section);
        ini_insert_key(&IniArena, &aoc_da_last(&sections).keys, test_key, test_value);

        // Naively printing the data
        foreach (&sections, i) {
                IniKeys keys = sections.data[i].keys;
                printf("Section: %s\n", sections.data[i].name.data);
                printf("  Keys: %d\n", (int)keys.len);
                foreach (&keys, j) {
                        printf("    %s = %s\n", keys.data[j].key.data, keys.data[j].value.data);
                }
        }

        ini_write(sections, file_path);

        // Expected output:
        //
        // Section: DEFAULT
        //   Keys: 2
        //     cwd = .
        //     user = aoc
        // Section: PATHS
        //   Keys: 3
        //     dir = ~/
        //     recurse = true
        //     epic = true

        aoc_arena_destroy(&IniArena);

        return 0;
}
