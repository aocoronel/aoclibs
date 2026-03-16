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

ForkCmd aoc_fork_cmd(char **ref argv);
PipeResult aoc_run_cmd(char **ref argv, const char *ref input);
rc aoc_read_fd(int fd);
int aoc_wait_for(pid_t pid);

#ifdef AOCLIBS_IMPLEMENTATION
#include "fork.c"
#endif // AOCLIBS_IMPLEMENTATION

#ifdef AOCLIBS_STRIP_PREFIX
#define fork_cmd aoc_fork_cmd
#define run_cmd aoc_run_cmd
#define read_fd aoc_read_fd
#define wait_for aoc_wait_for
#endif

#endif
