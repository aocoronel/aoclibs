#pragma once

#include "fork.h"

ForkCmd aoc_fork_cmd(char **argv) {
        int stdin_pipe[2], stdout_pipe[2];
        pipe(stdin_pipe);
        pipe(stdout_pipe);

        pid_t pid = fork();
        if (pid == -1) {
                close(stdin_pipe[0]);
                close(stdin_pipe[1]);
                close(stdout_pipe[0]);
                close(stdout_pipe[1]);
                return (ForkCmd){ -1, -1, -1 };
        }

        if (pid == 0) { // Child
                close(stdin_pipe[1]);
                close(stdout_pipe[0]);
                dup2(stdin_pipe[0], STDIN_FILENO);
                dup2(stdout_pipe[1], STDOUT_FILENO);
                close(stdin_pipe[0]);
                close(stdout_pipe[1]);
                execvp(argv[0], argv);
                exit(127);
        }

        // Parent
        close(stdin_pipe[0]);
        close(stdout_pipe[1]);
        return (ForkCmd){ pid, stdin_pipe[1], stdout_pipe[0] };
}

rc aoc_read_fd(int fd) {
        rc buff = { 0 };
        aoc_da_init(&buff, 512);

        char chunk[1024];
        ssize_t n;
        while ((n = read(fd, chunk, sizeof(chunk))) > 0) {
                aoc_rc_cat(&buff, chunk, n);
        }

        if (buff.data) {
                aoc_da_last(&buff) = '\0';
        }

        return buff;
}

int aoc_wait_for(pid_t pid) {
        int status;
        waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
}

PipeResult aoc_run_cmd(char **argv, const char *input) {
        ForkCmd fe = aoc_fork_cmd(argv);
        if (fe.pid == -1) return (PipeResult){};

        if (input) {
                write(fe.stdin_fd, input, strlen(input));
                close(fe.stdin_fd);
        }

        size_t len;
        rc output = aoc_read_fd(fe.stdout_fd);
        close(fe.stdout_fd);

        int status = aoc_wait_for(fe.pid);
        return (PipeResult){ output, status };
}
