#ifndef AOCLIBS_FORK_H_
#define AOCLIBS_FORK_H_

#include "base.h"
#include "rc.h"
#include <sys/types.h>

typedef struct {
	pid_t pid;
	int stdin_fd;
	int stdout_fd;
	int stderr_fd;
} fork_cmd_t;

typedef struct {
	rc out;
	rc err;
} fork_buff_t;

typedef struct {
	char **argv;
	char *input;
	bool out;
	bool err;
} ForkOptions;

typedef struct {
	rc out;
	rc err;
} CmdResult;

// run("ls", NULL, &output, .in = true, .out = true, .err = true)
AOCDEF int run_cmd(CmdResult *restrict out, ForkOptions opt);

AOCDEF fork_cmd_t fork_cmd(ForkOptions opt);
AOCDEF int wait_child(pid_t pid);

AOCDEF ssize_t write_fd(int fd, const void *buf, size_t count);
AOCDEF int read_fds(int out_fd, int err_fd, fork_buff_t *fb);
AOCDEF void close_fd(int fd);

#ifdef AOCLIBS_IMPLEMENTATION
#include "fork.c"
#endif // AOCLIBS_IMPLEMENTATION

#endif
