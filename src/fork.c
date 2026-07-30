#pragma once

#include "base.h"
#include "file.h"
#include "fork.h"
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

bool write_fd(int fd, const void *buf, size_t count) {
	$assert_nonnull(buf);

	const char *p = (const char *)buf;
	size_t left = count;

	while (left > 0) {
		ssize_t w = write(fd, p, left);
		if (w >= 0) {
			left -= (size_t)w;
			p += w;
			continue;
		}
		$catch(errno == EINTR) continue;
		$catch(errno == EAGAIN) return false;
		return false;
	}
	return true;
}

bool _fork_cmd(pid_t *pid, int fds[3], int pipes[6], Fork_Options opt) {
	char **argv = opt.argv;
	char *input = opt.input;

	$assert_nonnull(argv);

	int *in_pipe = pipes; // 0-1
	int *out_pipe = pipes + 2; // 2-3
	int *err_pipe = pipes + 4; // 4-5

	if (input && pipe(in_pipe) == -1) return false;
	if (opt.out && pipe(out_pipe) == -1) return false;
	if (opt.err && pipe(err_pipe) == -1) return false;

	*pid = fork();
	$catch(*pid == -1) return false;

	if (*pid == 0) { // child
		if (input) {
			close(in_pipe[1]);
			$catch(dup2(in_pipe[0], STDIN_FILENO) == -1) _exit(127);
		}
		if (opt.out) {
			close(out_pipe[0]);
			$catch(dup2(out_pipe[1], STDOUT_FILENO) == -1) _exit(127);
		}
		if (opt.err) {
			close(err_pipe[0]);
			$catch(dup2(err_pipe[1], STDERR_FILENO) == -1) _exit(127);
		}

		execvp(argv[0], argv);
		_exit(127);
	}
	if (input) close(in_pipe[0]);
	if (opt.out) close(out_pipe[1]);
	if (opt.err) close(err_pipe[1]);

	fds[0] = input ? in_pipe[1] : -1;
	fds[1] = opt.out ? out_pipe[0] : -1;
	fds[2] = opt.err ? err_pipe[0] : -1;

	if (input) {
		$catch(fds[0] == -1 || !write_fd(fds[0], input, strlen(input))) {
			kill(*pid, SIGTERM);
			wait_child(*pid);
			return false;
		}
		close(fds[0]);
	}

	return true;
}

#define $close_fd(fd) \
	if (fd != -1) close(fd);
Fork_Result fork_cmd(Fork_Options opt) {
	pid_t pid;
	int pipes[6] = { -1 };
	int fds[3];
	bool ret = _fork_cmd(&pid, fds, pipes, opt);

	if (!ret) {
		$close_fd(pipes[0]);
		$close_fd(pipes[1]);
		$close_fd(pipes[2]);
		$close_fd(pipes[3]);
		$close_fd(pipes[4]);
		$close_fd(pipes[5]);
		return (Fork_Result){ .pid = -1, .stdin_fd = -1, .stdout_fd = -1, .stderr_fd = -1 };
	}
	return (
	    Fork_Result){ .pid = pid, .stdin_fd = fds[0], .stdout_fd = fds[1], .stderr_fd = fds[2] };
}
#undef $close_fd

int read_fds(int out_fd, int err_fd, Cmd_Result *result) {
	$assert_nonnull(result);

	bool out_eof = (out_fd == -1);
	bool err_eof = (err_fd == -1);

	char buf[4096];

	if (!out_eof) da_init(&result->out, 4096);
	if (!err_eof) da_init(&result->err, 4096);

	while (!out_eof || !err_eof) {
		struct pollfd pfd[2];
		int nfds = 0;

		if (!out_eof) pfd[nfds++] = (struct pollfd){ out_fd, POLLIN, 0 };
		if (!err_eof) pfd[nfds++] = (struct pollfd){ err_fd, POLLIN, 0 };

		int ret = poll(pfd, nfds, -1);
		$catch(ret == -1) {
			if (errno == EINTR) continue;
			return -1;
		}

		for (int i = 0; i < nfds; i++) {
			if (!(pfd[i].revents & (POLLIN | POLLHUP | POLLERR))) continue;

			ssize_t n;
			while ((n = read(pfd[i].fd, buf, sizeof(buf))) == -1 && errno == EINTR)
				;

			if (n > 0) {
				if (pfd[i].fd == out_fd)
					da_append(&result->out, buf, n);
				else
					da_append(&result->err, buf, n);
			} else {
				close(pfd[i].fd);
				if (pfd[i].fd == out_fd)
					out_eof = true;
				else
					err_eof = true;
			}
		}
	}
	return 0;
}

int wait_child(pid_t pid) {
	int status;
	while (waitpid(pid, &status, 0) == -1)
		if (errno != EINTR) return -1;
	if (WIFEXITED(status)) return WEXITSTATUS(status);
	return -1;
}

int run_cmd(Fork_Options opt) {
	if (!opt.result) {
		opt.err = false;
		opt.out = false;
	}
	Cmd_Result result = { 0 };
	Fork_Result fc = fork_cmd(opt);

	$catch(fc.pid == -1) {
		return fc.pid;
	}

	read_fds(fc.stdout_fd, fc.stderr_fd, &result);

	if (opt.out) da_add_null(&result.out);
	if (opt.err) da_add_null(&result.err);

	result.out = result.out;
	result.err = result.err;

	if (opt.result) {
		*opt.result = result;
	}

	return wait_child(fc.pid);
}
