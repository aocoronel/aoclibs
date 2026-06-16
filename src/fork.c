#pragma once

#include <sys/types.h>
#include "base.h"
#include "fork.h"
#include "file.h"
#include <errno.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

ssize_t write_fd(int fd, const void *buf, size_t count) {
	ASSERT_NONNULL(buf);

	const char *p = (const char *)buf;
	size_t left = count;

	while (left > 0) {
		ssize_t w = write(fd, p, left);
		if (w >= 0) {
			left -= (size_t)w;
			p += w;
			continue;
		}
		if (errno == EINTR) continue;
		if (errno == EAGAIN) return -2;
		return -1;
	}
	return (ssize_t)count;
}

void close_fd(int fd) {
	if (fd != -1) close(fd);
}

fork_cmd_t fork_cmd(char **argv, const char *input, ForkOptions opt) {
	ASSERT_NONNULL(argv);
	int stdin_fd, stdout_fd, stderr_fd;
	pid_t pid;

	int in_pipe[2] = { -1, -1 };
	int out_pipe[2] = { -1, -1 };
	int err_pipe[2] = { -1, -1 };

	if (input && pipe(in_pipe) == -1) goto err;
	if (opt.out && pipe(out_pipe) == -1) goto err;
	if (opt.err && pipe(err_pipe) == -1) goto err;

	pid = fork();
	if (pid == -1) goto err;

	if (pid == 0) { // child
		if (input) {
			close(in_pipe[1]);
			if (dup2(in_pipe[0], STDIN_FILENO) == -1) _exit(127);
		}
		if (opt.out) {
			close(out_pipe[0]);
			if (dup2(out_pipe[1], STDOUT_FILENO) == -1) _exit(127);
		}
		if (opt.err) {
			close(err_pipe[0]);
			if (dup2(err_pipe[1], STDERR_FILENO) == -1) _exit(127);
		}

		close_fd(in_pipe[0]);
		close_fd(in_pipe[1]);
		close_fd(out_pipe[0]);
		close_fd(out_pipe[1]);
		close_fd(err_pipe[0]);
		close_fd(err_pipe[1]);

		execvp(argv[0], argv);
		_exit(127);
	}
	if (input) close(in_pipe[0]);
	if (opt.out) close(out_pipe[1]);
	if (opt.err) close(err_pipe[1]);

	stdin_fd = input ? in_pipe[1] : -1;
	stdout_fd = opt.out ? out_pipe[0] : -1;
	stderr_fd = opt.err ? err_pipe[0] : -1;

	if (input) {
		if (stdin_fd == -1) goto err_stdin;
		if (write_fd(stdin_fd, input, strlen(input)) < 0) {
			goto err_stdin;
		}
		close(stdin_fd);
	}

	return (fork_cmd_t){
		.pid = pid, .stdin_fd = stdin_fd, .stdout_fd = stdout_fd, .stderr_fd = stderr_fd
	};

err_stdin:
	kill(pid, SIGTERM);
	wait_child(pid);

err:
	close_fd(in_pipe[0]);
	close_fd(in_pipe[1]);
	close_fd(out_pipe[0]);
	close_fd(out_pipe[1]);
	close_fd(err_pipe[0]);
	close_fd(err_pipe[1]);

	return (fork_cmd_t){ .pid = -1, .stdin_fd = -1, .stdout_fd = -1, .stderr_fd = -1 };
}

int read_fds(int out_fd, int err_fd, fork_buff_t *fb) {
	ASSERT_NONNULL(fb);

	bool out_eof = (out_fd == -1);
	bool err_eof = (err_fd == -1);

	int maxfd = -1;
	if (out_fd >= 0) maxfd = out_fd;
	if (err_fd >= 0 && err_fd > maxfd) maxfd = err_fd;

	char buf[4096];

	while (!out_eof || !err_eof) {
		fd_set set;
		FD_ZERO(&set);

		if (!out_eof) FD_SET(out_fd, &set);
		if (!err_eof) FD_SET(err_fd, &set);

		int ret = select(maxfd + 1, &set, NULL, NULL, NULL);
		if (ret == -1) {
			if (errno == EINTR) continue;
			return -1;
		}

		if (!out_eof && FD_ISSET(out_fd, &set)) {
			ssize_t n;
			while ((n = read(out_fd, buf, sizeof(buf))) == -1 && errno == EINTR)
				;

			if (n > 0) {
				da_append(&fb->out, buf, n);
			} else {
				out_eof = true;
				close(out_fd);
			}
		}

		if (!err_eof && FD_ISSET(err_fd, &set)) {
			ssize_t n;
			while ((n = read(err_fd, buf, sizeof(buf))) == -1 && errno == EINTR)
				;

			if (n > 0) {
				da_append(&fb->err, buf, n);
			} else {
				err_eof = true;
				close(err_fd);
			}
		}
	}
	return 0;
}

int wait_child(pid_t pid) {
	int status;
	while (waitpid(pid, &status, 0) == -1) {
		if (errno != EINTR) return -1;
	}
	if (WIFEXITED(status)) return WEXITSTATUS(status);
	return -1;
}

int _run_cmd(char **argv, const char *input, CmdResult *out, ForkOptions opt) {
	ASSERT_NONNULL(out);

	CmdResult result = { 0 };
	fork_cmd_t fc = fork_cmd(argv, input, opt);

	if (fc.pid == -1) {
		return fc.pid;
	}

	fork_buff_t fb = { 0 };

	read_fds(fc.stdout_fd, fc.stderr_fd, &fb);

	if (opt.out) da_add_null(&fb.out);
	if (opt.err) da_add_null(&fb.err);

	result.out = fb.out;
	result.err = fb.err;

	*out = result;

	return wait_child(fc.pid);
}
