#include <aoclibs/common.h>
#include <aoclibs/fs/types.h>
#include <aoclibs/fs/file.h>
#include <aoclibs/fs/dir.h>
#include <aoclibs/mem/str.h>
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>

#define DIR_WALKER_BUFF 4096

Err dir_walker(const char *path, bool recurse, dw_dir isdir, dw_reg isreg,
               dw_lnk islnk, dw_null isnull) {
        DIR *dir = opendir(path);
        if (!dir) werr(errno, "failed to open directory");

        struct dirent *entry;
        char fullpath[DIR_WALKER_BUFF];

        while ((entry = readdir(dir)) != NULL) {
                if (strcmp(entry->d_name, ".") == 0 ||
                    strcmp(entry->d_name, "..") == 0)
                        continue;

                snprintf(fullpath, sizeof(fullpath), "%s/%s", path,
                         entry->d_name);

                FileType file_t = file_check_ft(fullpath);

                switch (file_t) {
                case F_REG:
                        if (isreg != NULL) isreg(fullpath);
                        break;
                case F_DIR:
                        if (recurse)
                                dir_walker(fullpath, recurse, isdir, isreg,
                                           islnk, isnull);
                        if (isdir != NULL) isdir(fullpath);
                        break;
                case F_LNK:
                        if (islnk != NULL) islnk(fullpath);
                        break;
                case F_NULL:
                        if (isnull != NULL) isnull(fullpath);
                        break;
                default:
                        break;
                }
        }
        closedir(dir);
        return ok();
}
