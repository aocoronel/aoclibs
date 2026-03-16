#include <stdio.h>
#include "../include/file.h"

void if_null(const char *path) {
    printf("%s doesn't exist!\n", path);
}

void if_reg(const char *path) {
    printf("%s is a file!\n", path);
}

void if_dir(const char *path) {
    printf("%s is a dir!\n", path);
}

void if_link(const char *path) {
    printf("%s is a link!\n", path);
}

void if_empty(const char *path) {
    printf("%s dir is empty!\n", path);
}

int main(int argc, char *argv[]) {
    aoc_dir_walk("test", true, if_dir, if_reg, if_link, if_null, if_empty);
    aoc_dir_walk(".", true, NULL, NULL, NULL, if_null, NULL);
    return 0;
}
