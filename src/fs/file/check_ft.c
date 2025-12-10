#define _XOPEN_SOURCE 700

#include <aoclibs/fs/types.h>
#include <aoclibs/fs/file.h>
#include <stdbool.h>
#include <sys/stat.h>

FileType file_check_ft(const char *path) {
        struct stat st;
        if (lstat(path, &st) == -1) return F_NULL;

        if (S_ISREG(st.st_mode)) return F_REG;
        if (S_ISDIR(st.st_mode)) return F_DIR;
        if (S_ISLNK(st.st_mode)) return F_LNK;
        if (S_ISCHR(st.st_mode)) return F_CHR;
        if (S_ISBLK(st.st_mode)) return F_BLK;
        if (S_ISFIFO(st.st_mode)) return F_FIFO;
        if (S_ISSOCK(st.st_mode)) return F_SOCK;

        return F_NULL;
}
