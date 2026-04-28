#ifndef AOCLIBS_FORK_H_
#define AOCLIBS_FORK_H_

#include "base.h"
#include "rc.h"

typedef struct {
    rc data;
    int status;
} PipeResult;

typedef struct {
    pid_t pid;
    int stdin_fd;
    int stdout_fd;
} ForkCmd;

AOCDEF ForkCmd fork_cmd(char **argv);
AOCDEF PipeResult run_cmd(char **restrict argv, const char *null restrict input);
AOCDEF rc read_fd(int fd);
AOCDEF int wait_for(pid_t pid);

#ifdef AOCLIBS_IMPLEMENTATION
#include "fork.c"
#endif // AOCLIBS_IMPLEMENTATION

#endif
