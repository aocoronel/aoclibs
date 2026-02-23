#include "ini.h"
#include <stdio.h>

int main(int argc, char **argv) {
        const char *file_path = "basic.ini";

        FILE *fp = fopen(file_path, "w");
        fprintf(fp, "cwd = .\nuser = aoc\n[PATHS]\ndir = ~/\nrecurse = true\nepic = true\n");
        fclose(fp);

        Arena IniArena = { 0 };
        IniSections sections = read_ini_file(&IniArena, file_path);

        foreach (&sections, i) {
                IniKeys keys = sections.data[i].keys;
                printf("Section: %s\n", sections.data[i].name.data);
                printf("  Keys: %d\n", (int)keys.len);
                foreach (&keys, j) {
                        printf("    %s = %s\n", keys.data[j].key.data, keys.data[j].value.data);
                }
        }

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

        return 0;
}
