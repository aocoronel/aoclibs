#include <aoclibs/fs/dir.h>
#include <stdio.h>

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

int main(int argc, char *argv[]) {
        printf("Hi\n");
        dir_walker(".", true, if_dir, if_reg, if_link, if_null);
        dir_walker(".", true, NULL, NULL, NULL, if_null);
        return 0;
}
