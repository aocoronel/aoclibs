#ifndef AOCLIBS_FILE_H_
#define AOCLIBS_FILE_H_

#define _GNU_SOURCE
#include "base.h"
#include "rc.h"
#include "cstr.h"

#define AOC_MAX_PATH 4096

typedef enum FileType {
    F_NULL, // Doesn't exist
    F_REG, // Regular file
    F_DIR, // Directory
    F_LNK, // Symbolic Link
    F_FAIL, // Failed to stat
} FileType;

typedef struct {
    FileType type;
    const char *name;
    struct stat *stat;
} FileMetadata;

typedef void (*null dw_fn)(const FileMetadata *);

typedef struct DirWalker DirWalker;

struct DirWalker {
    bool metadata;
    void (*null isdir)(const FileMetadata *, DirWalker *);
    dw_fn islnk;
    dw_fn isnull;
    dw_fn isreg;
    void (*null isempty)(const char *path);
};

// dir_walk("test.md", .metadata = false);
#define dir_walk(path, ...) dir_walker(path, &(DirWalker){ __VA_ARGS__ })
// Walks into a directory and read it's content
//
// dir_walk can run four user provided functions based on each filetype:
// reg, directory, lnk and null.
//
// If the user prefers to ignore a certain filetype, the function values can be
// passed as NULL.
//
// When recurse is set to true, when a directory is found, dir_walk will recurse
// in it.
//
// Sets errno << opendir
AOCDEF bool dir_walker(const char *restrict path, DirWalker *restrict dw);

// Stat the file and return its type
//
// F_FAIL :: failed to stat. Sets errno << lstat
AOCDEF FileMetadata get_filedata(struct stat *restrict st, const char *restrict path);

// Reads file, splitting the read buffer by the delimiter.
// Returns how many bytes has been read.
//
// Allocates to lineptr. The user owns the allocation.
AOCDEF size_t read_by_delim(char **restrict lineptr,
                            size_t *restrict n,
                            const char delim,
                            FILE *restrict stream);

// Reads entire file from "filepath" into "lines"
AOCDEF bool read_entire_file(rc *null restrict lines, const char *restrict filepath);

AOCDEF bool expand_path(rc *restrict output, Slice *restrict path);

AOCDEF char *null make_path(char *restrict out,
                            const size_t size,
                            const Slice *restrict dirs,
                            const size_t dir_count);

#ifdef AOCLIBS_IMPLEMENTATION
#include "file.c"
#endif

#endif // AOCLIBS_FILE_H_
