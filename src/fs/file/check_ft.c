#define _XOPEN_SOURCE 700

#include <aoclibs/common.h>
#include <aoclibs/fs/types.h>
#include <aoclibs/fs/file.h>
#include <stdbool.h>
#include <sys/stat.h>

FileType file_check_ft(const char *_Nonnull path) {
        ASSERT(path != NULL, "%s", "passing NULL pointer to Nonnull parameter");

        struct stat st;
        if (lstat(path, &st) == -1) return F_NULL;

        if (S_ISREG(st.st_mode)) return F_REG;
        else if (S_ISDIR(st.st_mode)) return F_DIR;
        else if (S_ISLNK(st.st_mode)) return F_LNK;
        else if (S_ISCHR(st.st_mode)) return F_CHR;
        else if (S_ISBLK(st.st_mode)) return F_BLK;
        else if (S_ISFIFO(st.st_mode)) return F_FIFO;
        else if (S_ISSOCK(st.st_mode)) return F_SOCK;

        return F_NULL;
}
