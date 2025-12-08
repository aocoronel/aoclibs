#include <a_error_fn_err.h>
#include <a_error_fn_ok.h>
#include <a_fs_check_filetype.h>
#include <a_fs_dir_walker.h>
#include <a_fs_types.h>
#include <a_mem_fn_strcmp.h>
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>

Error dir_walker(const char *path, bool recurse, dw_dir isdir,
                dw_regular isregular, dw_symlink issymlink, dw_null isnull) {
        DIR *dir = opendir(path);
        if (!dir) err(-1, errno);

        struct dirent *entry;
        char fullpath[4096];

        while ((entry = readdir(dir)) != NULL) {
                if (a_strcmp(entry->d_name, ".") == 0 ||
                    a_strcmp(entry->d_name, "..") == 0)
                        continue;

                snprintf(fullpath, sizeof(fullpath), "%s/%s", path,
                         entry->d_name);

                enum FileType file_t = check_filetype(fullpath);

                switch (file_t) {
                case F_REGULAR:
                        if (isregular != NULL) isregular(fullpath);
                        break;
                case F_DIR:
                        if (recurse)
                                dir_walker(fullpath, recurse, isdir, isregular,
                                           issymlink, isnull);
                        if (isdir != NULL) isdir(fullpath);
                        break;
                case F_SYMLINK:
                        if (issymlink != NULL) issymlink(fullpath);
                        break;
                case F_NULL:
                        if (isnull != NULL) isnull(fullpath);
                        break;
                }
        }
        closedir(dir);
        return ok();
}
