/*
 *  Operation Systems (OSs) Course Assignment 2
 *  Shell program
 *  Copyright (C) 2023  Roy Simanovich and Linor Ronen
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include "stshell.h"

char *homedir = NULL;
char *workingdir = NULL;
char *cwd = NULL;

void sigint_handler(int sig) {
	// Do absolutely nothing
	if (sig == SIGINT)
	{
		if (DEBUG_MODE)
			fprintf(stdout, "Interrupt signal\n");

		return;
	}
}

int main(void) {
	char* argv[MAX_ARGS + 1];
	char command[MAX_COMMAND_LENGTH] = { 0 };

	cwd = (char*) malloc(MAX_COMMAND_LENGTH * sizeof(char));

	if (cwd == NULL)
	{
		fprintf(stderr, "%s: malloc error\n", ERR_SYSCALL);
		return EXIT_FAILURE;
	}

	homedir = getenv("HOME");

	// Ignore SIGINT
	signal(SIGINT, sigint_handler);

	// Print copyright and license
	fprintf(stdout,
			"Student Shell  Copyright (C) 2023  Roy Simanovich and Linor Ronen\n"
			"This program comes with ABSOLUTELY NO WARRANTY.\n"
			"This is free software, and you are welcome to redistribute it\n"
			"under certain conditions; type `gnu' for details.\n\n");

	while (1)
	{
		if (getcwd(cwd, MAX_COMMAND_LENGTH) == NULL)
		{
			fprintf(stderr, "%s\n", ERR_SYSCALL);
			return EXIT_FAILURE;
		}

		// Print the prompt.
		fprintf(stdout, "%s %s", cwd, "> ");
		fflush(stdout);

		// Read a command from the user.
		fgets(command, MAX_COMMAND_LENGTH, stdin);
		
		// Check if the command is an internal command.
		if (parse_command(command, argv) == Internal)
		{
			if (DEBUG_MODE)
				fprintf(stdout, "Finished executing internal command.\n");

			continue;
		}

		// This is an external command, so we need to execute it with execvp.
		execute_command(argv);

		if (DEBUG_MODE)
			fprintf(stdout, "Finished executing external command.\n");
	}

	free(cwd);

	return EXIT_SUCCESS;
}

CommandType parse_command(char* command, char** argv) {
	char* token = NULL;
	int words = 0;
	size_t i = 0, j = 0;

	// Count the number of words.
	for (size_t k = 0; k < strlen(command); ++k)
	{
		if (*(command + k) == ' ' && *(command + k + 1) != ' ')
			++words;
	}

	++words;

	// Remove quotes from the command.
	for (i = 0; i < strlen(command); ++i)
	{
		if (*(command + i) != '"' && *(command + i) != '\\')
			*(command + j++) = *(command + i);

		else if (*(command + i + 1) == '"' && *(command + i) == '\\')
			*(command + j++) = '"';

		else if (*(command + i + 1) != '"' && *(command + i) == '\\')
			*(command + j++) = '\\';
	}

	if (j > 0)
		*(command + j) = '\0';

	// Parse the command - first parse according to \n and then according to " ".
	command = strtok(command, "\n");
	token = strtok(command, " ");

	// Exit command.
	if (strcmp(token, CMD_EXIT) == 0)
	{
		free(cwd);
        exit(EXIT_SUCCESS);
	}

	// Change directory command.
	else if (strcmp(token, CMD_CD) == 0)
	{
		token = strtok(NULL, " ");
		cmdCD(token, words);
		return Internal;
	}

	// Clean screen command.
	else if (strcmp(token, CMD_CLEAR) == 0)
	{
		write(STDOUT_FILENO, CMD_CLEAR_FLUSH, CMD_CLEAR_FLUSH_LEN);
		return Internal;
	}

	// Print working directory command.
	else if (strcmp(token, CMD_PWD) == 0)
	{
		fprintf(stdout, "%s\n", cwd);
		return Internal;
	}

	// GNU license command.
	else if (strcmp(token, CMD_GNU) == 0)
	{
		fprintf(stdout, "%s\n", CMD_GNU_LICENSE);
		return Internal;
	}

	// Parse the arguments (if there are any).
	while (token != NULL)
	{
		*argv++ = token;
		token = strtok(NULL, " ");
	}

	*argv = NULL;

	// This is an external command.
	return External;
}

Result cmdCD(char *path, int argc) {
	if (argc > 2)
	{
		fprintf(stderr, "%s\n", ERR_CMD_CD_ARG);
		return Failure;
	}

	else if (argc == 2)
	{
		// Home directory.
		if (strcmp(path, "~") == 0)
		{
			getcwd(workingdir, MAX_PATH_LENGTH);

			if (chdir(homedir) == -1)
			{
				fprintf(stderr, "%s\n", ERR_SYSCALL);
				return Failure;
			}

			return Success;
		}

		else if (strcmp(path, "-") == 0)
		{
			if (strcmp(workingdir, "") == 0)
				return Success;

			if (chdir(workingdir) == -1)
			{
				fprintf(stderr, "%s\n", ERR_SYSCALL);
				return Failure;
			}
		}

		getcwd(workingdir, MAX_PATH_LENGTH);

		if (chdir(path) == -1)
		{
			fprintf(stderr, "%s\n", ERR_CMD_CD);
			return Failure;
		}		
	}

	else
	{
		getcwd(workingdir, MAX_PATH_LENGTH);

		if (chdir(homedir) == -1)
		{
			fprintf(stderr, "%s\n", ERR_SYSCALL);
			return Failure;
		}
	}

	return Success;
}

void execute_command(char** argv) {
	pid_t pid;
    int status;
    int waited;

	pid = fork();

	if (pid < 0)
	{
		fprintf(stderr, "%s\n", ERR_SYSCALL);
		exit(EXIT_FAILURE);
	}

	if (pid == 0)
	{
		// Child process, execute the command, and print an error if it fails.
		if (execvp(*argv, argv) == -1)
		{
			fprintf(stderr, "%s: %s\n", ERR_SYSCALL, strerror(errno));
			exit(EXIT_FAILURE);
		}

		// Exit the child process, since execvp replaces the child process with the command.
		exit(EXIT_SUCCESS);
	}

	else
	{
		// Parent process, wait for the child process to finish.
		do
		{
			waited = waitpid(pid, &status, WNOHANG);
		}
		
		while (waited != pid);

		if (DEBUG_MODE)
		{
			if (WIFEXITED(status))
				fprintf(stdout, "Child process (%d) exited normally with status %d.\n", pid, WEXITSTATUS(status));

			else if (WIFSIGNALED(status))
				fprintf(stdout, "Child process (%d) was terminated by a signal (%d).\n", pid, WTERMSIG(status));

			else if (WIFSTOPPED(status))
				fprintf(stdout, "Child process (%d) was stopped by a signal (%d).\n", pid, WSTOPSIG(status));
		}
	}
}