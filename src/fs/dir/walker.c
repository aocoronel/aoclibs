#include <aoclibs/common.h>
#include <aoclibs/fs/dir.h>
#include <aoclibs/fs/file.h>
#include <aoclibs/fs/types.h>
#include <aoclibs/mem/str.h>
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define DIR_WALKER_BUFF 4096

Err dir_walker(const char *_Nonnull path, bool recurse, dw_dir isdir, dw_reg isreg,
               dw_lnk islnk, dw_null isnull, dw_empty isempty) {
        ASSERT(path != NULL, "%s", "passing NULL pointer to Nonnull parameter");

        DIR *dir = opendir(path);
        if (!dir) return ErrErrno;

        u8 empty = 0;

        struct dirent *entry;
        char fullpath[DIR_WALKER_BUFF];

        while ((entry = readdir(dir)) != NULL) {
                if (mem_streq(entry->d_name, ".") == 0 ||
                    mem_streq(entry->d_name, "..") == 0)
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
                                           islnk, isnull, isempty);
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

                empty++;
        }

        if (empty == 0 && isempty != NULL) isempty(path);

        closedir(dir);
        return ok();
}
