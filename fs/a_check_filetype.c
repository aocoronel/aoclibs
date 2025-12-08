#define _XOPEN_SOURCE 700
#include <a_fs_check_filetype.h>
#include <stdbool.h>
#include <sys/stat.h>

enum FileType check_filetype(const char *path) {
        struct stat st;
        if (lstat(path, &st) == -1) return F_NULL;

        if (S_ISDIR(st.st_mode)) return F_DIR;
        if (S_ISREG(st.st_mode)) return F_REGULAR;
        if (S_ISLNK(st.st_mode)) return F_SYMLINK;

        return F_NULL;
}
