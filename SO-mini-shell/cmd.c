// SPDX-License-Identifier: BSD-3-Clause

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmd.h"
#include "utils.h"

#define READ		0
#define WRITE		1

/**
 * Internal change-directory command.
 */
static int shell_cd(word_t *dir)
{
	/* TODO: Execute cd. */
	if (dir == NULL || dir->next_word != NULL)
		return 1;

	int rc;

	rc = chdir(dir->string);

	if (rc < 0)
		return 1;
	return 0;
}

/**
 * Internal exit/quit command.
 */
static int shell_exit(void)
{
	/* TODO: Execute exit/quit. */
	return SHELL_EXIT; /* TODO: Replace with actual exit code. */
}

static int shell_pwd(void)
{
	char cwd[1024];

	if (getcwd(cwd, sizeof(cwd)) != NULL) {
		fprintf(stdout, "%s\n", cwd);
		return 0;
	}
	return 1;
}

void open_input_file(word_t *in)
{
	char *input_filename = get_word(in);
	int input_fd, input_rc;

	input_fd = open(input_filename, O_RDONLY, 0666);
	DIE(input_fd < 0, "open input file");

	input_rc = dup2(input_fd, STDIN_FILENO);
	DIE(input_rc < 0, "dup2 input file");

	input_rc = close(input_fd);
	DIE(input_rc < 0, "close input file");

	free(input_filename);
}

void open_output_file(word_t *out, int flags)
{
	char *output_filename = get_word(out);
	int output_fd, output_rc = 0;

	if (flags == IO_OUT_APPEND)
		output_fd = open(output_filename, O_WRONLY | O_CREAT | O_APPEND, 0666);
	else
		output_fd = open(output_filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	DIE(output_fd < 0, "open output file");

	output_rc = dup2(output_fd, STDOUT_FILENO);
	DIE(output_rc < 0, "dup2 output file");

	output_rc = close(output_fd);
	DIE(output_rc < 0, "close output file");

	free(output_filename);
}

void open_error_file(word_t *out, word_t *err, int flags)
{
	char *error_filename = get_word(err);
	char *output_filename = NULL;
	int error_rc, error_fd;

	if (out != NULL)
		output_filename = get_word(out);

	if (output_filename != NULL && !strcmp(output_filename, error_filename)) {
		error_rc = dup2(STDOUT_FILENO, STDERR_FILENO);
		DIE(error_rc < 0, "dup2 error file");
	} else {
		if (flags == IO_ERR_APPEND)
			error_fd = open(error_filename, O_WRONLY | O_CREAT | O_APPEND, 0666);
		else
			error_fd = open(error_filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);

		DIE(error_fd < 0, "open error file");

		error_rc = dup2(error_fd, STDERR_FILENO);
		DIE(error_rc < 0, "dup2 error file");

		error_rc = close(error_fd);
		DIE(error_rc < 0, "close error file");
	}

	free(error_filename);
	if (output_filename != NULL)
		free(output_filename);
}

static int executable_exists(const char *executable)
{
	char command[256];

	snprintf(command, sizeof(command), "which %s", executable);

	FILE *fp = popen(command, "r");

	DIE(fp == NULL, "popen");

	char path[256];

	if (fgets(path, sizeof(path), fp) == NULL) {
		pclose(fp);
		return 0;
	}

	pclose(fp);
	return 1;
}

/**
 * Parse a simple command (internal, environment variable assignment,
 * external command).
 */
static int parse_simple(simple_command_t *s, int level, command_t *father)
{
	/* TODO: Sanity checks. */
	if (s == NULL)
		return shell_exit();


	if (!strcmp(s->verb->string, "true"))
		return 0;

	if (!strcmp(s->verb->string, "false"))
		return 1;

	/* TODO: If builtin command, execute the command. */
	if (strcmp(s->verb->string, "exit") == 0 || strcmp(s->verb->string, "quit") == 0)
		return shell_exit();

	if (strcmp(s->verb->string, "cd") == 0) {
		if (s->out != NULL) {
			char *output_filename = get_word(s->out);
			int output_fd;

			if (s->io_flags == IO_OUT_APPEND)
				output_fd = open(output_filename, O_WRONLY | O_CREAT | O_APPEND, 0666);
			else
				output_fd = open(output_filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);

			DIE(output_fd < 0, "open output file");
			free(output_filename);
		}

		if (s->err != NULL) {
			char *error_filename = get_word(s->err);
			int error_fd;

			if (s->io_flags == IO_ERR_APPEND)
				error_fd = open(error_filename, O_WRONLY | O_CREAT | O_APPEND, 0666);
			else
				error_fd = open(error_filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);

			DIE(error_fd < 0, "open error file");

			free(error_filename);
		}
		return shell_cd(s->params);
	}

	/* TODO: If variable assignment, execute the assignment and return
	 * the exit status.
	 */
	if (s->verb->next_part != NULL && strcmp(s->verb->next_part->string, "=") == 0) {
		int rc;
		char *new_value = get_word(s->verb->next_part->next_part);

		rc = setenv(s->verb->string, new_value, 1);
		free(new_value);
		return rc;
	}

	/* TODO: If external command:
	 *   1. Fork new process
	 *     2c. Perform redirections in child
	 *     3c. Load executable in child
	 *   2. Wait for child
	 *   3. Return exit status
	 */
	pid_t new_process_pid;
	pid_t ret_pid;
	int status;

	int number_of_arguments = 0;

	char **argument_list = get_argv(s, &number_of_arguments);
	const char *command = s->verb->string;
	int child_error;

	new_process_pid = fork();

	switch (new_process_pid) {
	case -1:
		DIE(1, "fork");
		break;
	case 0:
		/* Implement logic for child process */
		if (s->in != NULL)
			open_input_file(s->in);

		if (s->out != NULL)
			open_output_file(s->out, s->io_flags);

		if (s->err != NULL)
			open_error_file(s->out, s->err, s->io_flags);

		if (strcmp(s->verb->string, "pwd") == 0) {
			int result = shell_pwd();

			exit(result);
		}

		if (executable_exists(command)) {
			child_error = execvp(command, argument_list);
			exit(child_error);
		} else {
			fprintf(stderr, "Execution failed for '%s'\n", command);
			exit(1);
		}
	default:
		/* Implement logic for parent process */
		ret_pid = waitpid(new_process_pid, &status, 0);
		free(argument_list);
		DIE(ret_pid < 0, "waitpid parent error");

		if (WIFEXITED(status))
			return WEXITSTATUS(status);
		else
			return -1;
	}
}

/**
 * Process two commands in parallel, by creating two children.
 */
static int run_in_parallel(command_t *cmd1, command_t *cmd2, int level,
		command_t *father)
{
	/* TODO: Execute cmd1 and cmd2 simultaneously. */
	int result, status;
	pid_t pid;

	pid = fork();
	switch (pid) {
	case -1:
		DIE(1, "fork");
		return SHELL_EXIT;
	case 0:
		result = parse_command(cmd1, level, father);
		exit(result);
	default:
		result = parse_command(cmd2, level, father);
		int ret_pid = waitpid(pid, &status, 0);

		DIE(ret_pid < 0, "waitpid error");

		if (WIFEXITED(status))
			return result && WEXITSTATUS(status);
		else
			return 1;
	}
}

/**
 * Run commands by creating an anonymous pipe (cmd1 | cmd2).
 */
static bool run_on_pipe(command_t *cmd1, command_t *cmd2, int level,
		command_t *father)
{
	/* TODO: Redirect the output of cmd1 to the input of cmd2. */
	int pipedes[2], rc;

	rc = pipe(pipedes);
	DIE(rc < 0, "pipe");

	pid_t ppid;

	ppid = fork();
	int stdin_fd, result;

	switch (ppid) {
	case -1:
		close(pipedes[READ]);
		close(pipedes[WRITE]);
		DIE(1, "fork");
		return SHELL_EXIT;

	case 0:
		rc = close(pipedes[READ]);
		DIE(rc < 0, "close");

		rc = dup2(pipedes[WRITE], STDOUT_FILENO);
		DIE(rc < 0, "dup2");

		rc = close(pipedes[WRITE]);
		DIE(rc < 0, "close");

		result = parse_command(cmd1, level, father);

		exit(result);
	default:
		rc = close(pipedes[WRITE]);
		DIE(rc < 0, "close");

		stdin_fd = dup(STDIN_FILENO);
		DIE(stdin_fd < 0, "dup");

		rc = dup2(pipedes[READ], STDIN_FILENO);
		DIE(rc < 0, "dup2");

		rc = close(pipedes[READ]);
		DIE(rc < 0, "close");

		result = parse_command(cmd2, level, father);

		int status;

		waitpid(ppid, &status, 0);

		rc = dup2(stdin_fd, STDIN_FILENO);
		DIE(rc < 0, "dup2");

		rc = close(stdin_fd);
		DIE(rc < 0, "close");
		return result;
	}
}

/**
 * Parse and execute a command.
 */
int parse_command(command_t *c, int level, command_t *father)
{
	/* TODO: sanity checks */

	if (c->op == OP_NONE)
		return parse_simple(c->scmd, level, father);

	int result;

	switch (c->op) {
	case OP_SEQUENTIAL:
		/* TODO: Execute the commands one after the other. */
		parse_command(c->cmd1, level + 1, c);
		parse_command(c->cmd2, level + 1, c);
		break;

	case OP_PARALLEL:
		/* TODO: Execute the commands simultaneously. */
		return run_in_parallel(c->cmd1, c->cmd2, level + 1, c);

	case OP_CONDITIONAL_NZERO:
		/* TODO: Execute the second command only if the first one
		 * returns non zero.
		 */
		result = parse_command(c->cmd1, level + 1, c);
		if (result != 0)
			return parse_command(c->cmd2, level + 1, c);
		return result;

	case OP_CONDITIONAL_ZERO:
		/* TODO: Execute the second command only if the first one
		 * returns zero.
		 */
		result = parse_command(c->cmd1, level + 1, c);
		if (result == 0)
			return parse_command(c->cmd2, level + 1, c);
		return result;

	case OP_PIPE:
		/* TODO: Redirect the output of the first command to the
		 * input of the second.
		 */
		return run_on_pipe(c->cmd1, c->cmd2, level + 1, c);

	default:
		return SHELL_EXIT;
	}

	return 0; /* TODO: Replace with actual exit code of command. */
}
