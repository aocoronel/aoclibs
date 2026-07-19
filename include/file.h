#ifndef AOC_FILE_H_
#define AOC_FILE_H_

#include "base.h"

#include "rc.h"
#include "cstr.h"
#include <sys/stat.h>

enum File_Type {
	F_NULL, // Doesn't exist
	F_REG, // Regular file
	F_DIR, // Directory
	F_LNK, // Symbolic Link
	F_FAIL, // Failed to stat
};

typedef void (*Dir_Walker_Fn)(File_Type, struct stat *, const char *);

struct Dir_Walker {
	void (*null isdir)(File_Type, Dir_Walker *);
	Dir_Walker_Fn null islnk;
	Dir_Walker_Fn null isnull;
	Dir_Walker_Fn null isreg;
	void (*null isempty)(const char *path);
};

// Usage: dir_walk("test.md", .islnk = my_fn);
#define dir_walk(path, ...)                             \
	do {                                                \
		Dir_Walker _walk = (Dir_Walker){ __VA_ARGS__ }; \
		dir_walker(path, &_walk);                       \
	} while (0)
// dir_walker can run five user provided functions based on each File_Type.
//
// If the user prefers to ignore a certain filetype, the function values can be
// passed as NULL.
//
// dir_walker will not recurse by itself. You have to call dir_walker again inside
// the isdir function
//
// Sets errno << opendir
AOCDEF bool dir_walker(const char *restrict path, Dir_Walker *restrict dw);

// Stat the file and return its type
//
// F_FAIL :: failed to stat. Sets errno << lstat
AOCDEF File_Type get_filetype(struct stat *restrict st, const char *restrict path);

// Reads file, splitting the read buffer by the delimiter.
// Returns how many bytes has been read.
//
// Allocates to "buff". The user owns the allocation.
// The buffer is null terminated
//
// SIZE_MAX :: failed to allocate
// SIZE_MAX :: EOF
AOCDEF size_t read_by_delim(char **restrict buff,
							size_t *restrict size,
							const char delim,
							FILE *restrict fd);

// Reads stream by lines.
// The slice is null terminated.
AOCDEF bool
read_by_lines(Slice *out, char **restrict buff, size_t *restrict size, FILE *restrict fd);

// Reads entire file from "filepath" into "lines"
AOCDEF bool read_entire_file(rc *null restrict lines, const char *restrict filepath);

// Returns the path corresponding to $HOME environment variable
// This function stores the getenv() output into a `static const char *` variable
//
// NULL :: failed to get env
AOCDEF const char *null get_home_env();

// Returns string to "output" containing the absolute path from "path"
// Expands these special paths: "~/", "." and "./"
// Expands environment variables
// The output is not NULL-terminated
// If the "output" already has allocated memory, and it fails, no memory is freed
//
// 0 :: OK
// 1 :: Failed to get environment variables
// 2 :: Failed to get current working directory
AOCDEF int absolute_path_from(rc *restrict output, Slice *restrict path);

// Turns a path into a array of slices containing each directory that composes a path
//
// Memory is allocated in "out" and can be freed using free()
//
// 0 :: nothing to do
// >0 :: slice count
AOCDEF size_t dismantle_path(Slice **restrict out, const char *restrict path, size_t len);

// Turns a array of slices containing each directory that composes a path into a path
// The path is null-terminated, and is set into "out"
//
// NULL :: path doesn't fit in "size"
AOCDEF char *null make_path(char *restrict out,
							const size_t size,
							const Slice *restrict dirs,
							const size_t dir_count);

#ifdef AOC_IMPLEMENTATION
#include "file.c"
#endif

#endif // AOC_FILE_H_
