#include "base.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "da.h"
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

ForkCmd aoc_fork_cmd(char **argv);
PipeResult aoc_run_cmd(char **argv, const char *input);
rc aoc_read_fd(int fd);
int aoc_wait_for(pid_t pid);

#ifdef AOCLIBS_IMPLEMENTATION
#include "fork.c"
#endif // AOCLIBS_IMPLEMENTATION
