#ifndef AOCLIBS_FS_PATH_H
#define AOCLIBS_FS_PATH_H

#include <aoclibs/common.h>
#include <aoclibs/mem/str.h>
#include <aoclibs/tunit.h>

#define mem_basename(s) _mem_basename(s, mem_strlen(s))
#define mem_dirname(s) _mem_dirname(s, mem_strlen(s))

/*
 * Returns the basename from a given path (s)
*/
char *_mem_basename(char *_Nonnull s, usize s_len);

#ifdef TUNIT
#include <string.h>
TEST(mem_basename) {
        char test[] = "Hello/world/world2/world3/world_3htree/dog";
        const char *test_basename = mem_basename(test);
        TASSERT(strcmp("dog", test_basename) != -1,
                "basename should return string after last '/': dog");
        tunit_log("Normal: %s\nBasename: %s\n", test, test_basename);
}
#endif

/*
 * Returns the dirname from a given path (s)
*/
char *_mem_dirname(char *_Nonnull mut s, usize s_len);

#ifdef TUNIT
TEST(mem_dirname) {
        char test[] = "Hello/world/world2/world3/world_3htree/dog";
        const char *test_dirname = mem_dirname(test);
        TASSERT(strcmp("Hello/world/world2/world3/world_3htree",
                       test_dirname) != -1,
                "dirname should return string before last '/': Hello/world/world2/world3/world_3htree/");
        tunit_log("Normal: %s\nDirname: %s\n", test, test_dirname);
        TASSERT(strcmp(test, "Hello/world/world2/world3/world_3htree") != -1,
                "dirname modifies the orignal string");
}
#endif

#endif
