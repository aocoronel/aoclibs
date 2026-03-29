#ifndef AOCLIBS_FORK_H_
#define AOCLIBS_FORK_H_

#include "base.h"
#include "rc.h"
#include <sys/types.h>

typedef struct {
    rc data;
    int status;
} PipeResult;

typedef struct {
    pid_t pid;
    int stdin_fd;
    int stdout_fd;
} ForkCmd;

fn ForkCmd fork_cmd(char **argv);
fn PipeResult run_cmd(char **argv, const char *null input);
fn rc read_fd(int fd);
fn int wait_for(pid_t pid);

#ifdef AOCLIBS_IMPLEMENTATION
#include "fork.c"
#endif // AOCLIBS_IMPLEMENTATION

#endif
