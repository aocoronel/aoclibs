#ifndef AOC_FORK_H_
#define AOC_FORK_H_

#include "base.h"

#include "rc.h"
#include <sys/types.h>

struct Fork_Result {
	pid_t pid;
	int stdin_fd;
	int stdout_fd;
	int stderr_fd;
};

struct Cmd_Result {
	rc out;
	rc err;
};

struct Fork_Options {
	Cmd_Result *null result;
	char **argv;
	char *input;
	bool out;
	bool err;
};

// Passing "opt.result" as NULL implies "out" and "err" to be false
// Only use "err" and "out" if you need to capture the results
AOCDEF int run_cmd(Fork_Options opt);

AOCDEF Fork_Result fork_cmd(Fork_Options opt);
AOCDEF int wait_child(pid_t pid);

AOCDEF bool write_fd(int fd, const void *buf, size_t count);
AOCDEF int read_fds(int out_fd, int err_fd, Cmd_Result *fb);
AOCDEF void close_fd(int fd);

#ifdef AOC_IMPLEMENTATION
#include "fork.c"
#endif // AOC_IMPLEMENTATION

#endif
