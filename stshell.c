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
#include <fcntl.h>
#include <errno.h>
#include <strings.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include "stshell.h"

char *homedir = NULL;
char *workingdir = NULL;
char *cwd = NULL;

int main(void) {
	char **argv = NULL;
	char command[MAX_COMMAND_LENGTH + 1] = { 0 };

	cwd = (char*) calloc((MAX_PATH_LENGTH + 1), sizeof(char));
	argv = (char**) calloc((MAX_ARGS + 1), sizeof(char*));

	if (cwd == NULL || argv == NULL)
	{
		fprintf(stderr, "%s: calloc error (%s)\n", ERR_SYSCALL, strerror(errno));
		return EXIT_FAILURE;
	}

	homedir = getenv("HOME");

	// Ignore SIGINT
	signal(SIGINT, SIG_IGN);

	// Print copyright and license
	fprintf(stdout,
			"Student Shell  Copyright (C) 2023  Roy Simanovich and Linor Ronen\n"
			"This program comes with ABSOLUTELY NO WARRANTY.\n"
			"This is free software, and you are welcome to redistribute it\n"
			"under certain conditions; type `gnu' for details.\n\n");

	while (1)
	{
		if (getcwd(cwd, MAX_PATH_LENGTH) == NULL)
		{
			fprintf(stderr, "%s\n", ERR_SYSCALL);
			return EXIT_FAILURE;
		}

		for (size_t i = 0; i < MAX_ARGS; ++i)
		{
			*(argv + i) = (char*) calloc((MAX_ARG_LENGTH + 1), sizeof(char));

			if (*(argv + i) == NULL)
			{
				fprintf(stderr, "%s: calloc error (%s)\n", ERR_SYSCALL, strerror(errno));;

				// Memory cleanup in case of calloc error.
				for (size_t j = 0; j < i; ++j)
					free(*(argv + j));

				free(argv);
				free(cwd);

				return EXIT_FAILURE;
			}
		}

		// Prepare to print beutiful prompt.
		char* user_name = getenv("USER");
		char* cwd_changed = (char*) malloc((MAX_PATH_LENGTH + 1) * sizeof(char));
		strcpy(cwd_changed + 1, cwd + strlen(homedir));
		*cwd_changed = '~';

		// Print the prompt.
		fprintf(stdout, "%s@%s%s", user_name, (strstr(cwd, homedir) == NULL ? cwd:cwd_changed), "$ ");
		fflush(stdout);

		// Free the memory allocated for the prompt.
		free(cwd_changed);

		// Read a command from the user.
		fgets(command, MAX_COMMAND_LENGTH, stdin);
		
		// Check if the command is an internal command.
		if (parse_command(command, argv) == Internal)
		{
			if (DEBUG_MODE)
				fprintf(stdout, "Finished executing internal command.\n");

			for (size_t i = 0; i < MAX_ARGS; ++i)
				free(*(argv + i));

			continue;
		}

		// This is an external command, so we need to execute it with execvp.
		execute_command(argv);

		if (DEBUG_MODE)
			fprintf(stdout, "Finished executing external command.\n");

		// Clear the arguments.
		for (size_t i = 0; i < MAX_ARGS; ++i)
			free(*(argv + i));

		// Clear the command.
		bzero(command, (MAX_COMMAND_LENGTH + 1));
	}

	// Memory cleanup.
	free(argv);
	free(cwd);

	return EXIT_SUCCESS;
}

char *append(char before, char *str, char after) {
    size_t len = strlen(str);

    if(before)
    {
		for (size_t i = len; i > 0; --i)
			*(str + i) = *(str + i - 1);

    	*str = before;
    }

    if(after)
    {
        *(str + len) = after;
        *(str + len + 1) = '\0';
    }

    return str;
}

CommandType parse_command(char* command, char** argv) {
	char **pargv = argv;
	char* token = NULL;
	int words = 1;
	size_t i = 0, j = 0;

	// Remove the newline character from the command, to check if it's empty.
	command = strtok(command, "\n");

	// Safe-fail if the command is empty, to avoid segmentation fault.
	if (command == NULL || strlen(command) == 0)
		return Internal;

	// Count the number of words.
	for (size_t k = 0; k < strlen(command); ++k)
	{
		if (*(command + k) == ' ' && *(command + k + 1) != ' ')
			++words;
	}

	if (words > MAX_ARGS)
	{
		fprintf(stderr, "%s: %s\n", ERR_SYSCALL, ERR_TOO_MANY_ARGS);
		return Internal;
	}

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

	// Parse the first argument (the command itself).
	token = strtok(command, " ");

	// Exit command.
	if (strcmp(token, CMD_EXIT) == 0)
	{
		// Free the memory allocated for the arguments.
		for (size_t i = 0; i < MAX_ARGS; ++i)
			free(*(argv + i));

		// Free the memory allocated for the arguments array.
		free(argv);

		// Free the memory allocated for the current working directory.
		free(cwd);

		// Exit the program.
        exit(EXIT_SUCCESS);
	}

	// Change directory command.
	else if (strcmp(token, CMD_CD) == 0)
	{
		// Get the next token, which is the directory to change to.
		token = strtok(NULL, " ");
		cmdCD(token, words);
		return Internal;
	}

	// Clean screen command.
	else if (strcmp(token, CMD_CLEAR) == 0)
	{
		// Write the clear screen command to the standard output.
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
		memcpy(*pargv, token, strlen(token));
		++pargv;
		token = strtok(NULL, " ");
	}

	// Set the last argument to NULL, as required by execvp.
	// By definition, the last argument must be NULL, as we allocate just enough memory for the arguments.
	// We also make sure to free the memory allocated for the last argument, to avoid memory leaks.
	if (*pargv != NULL)
	{
		free(*pargv);
		*pargv = NULL;
	}

	// Check if the command is a special command (cmp, copy, encode, decode).
	// If it is, we need to append the current working directory to the command.
	// This is because the special commands are located in the same directory as the shell.
	if (strcmp(*argv, CMD_CMP) == 0 || strcmp(*argv, CMD_COPY) == 0 || strcmp(*argv, CMD_ENCODE) == 0 || strcmp(*argv, CMD_DECODE) == 0)
	{
		if (DEBUG_MODE)
			fprintf(stdout, "Special internal command: %s\n", *argv);

		// Append the current working directory to the command.
		*argv = append('/', *argv, '\0');
		*argv = append('.', *argv, '\0');

		// Execute the command as an external command.
		// Since the command is actually run from different program, we need to use execvp.
		// We also need to pass the arguments to the program.
		execute_command(argv);

		// We still need to return Internal, because we want the shell to continue running and not accidentally run the command again.
		return Internal;
	}

	// This is an external command.
	return External;
}

Result cmdCD(char *path, int argc) {
	// Only one argument is allowed, like in the original shell.
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

		// Previous directory.
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

	// No arguments - go to home directory.
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
	int status, i = 0, num_pipes = 0, num_redirects = 0;

	if (DEBUG_MODE)
	{
		fprintf(stdout, "Executing external command: %s.\n",*argv);

		for (int i = 1; i < MAX_ARGS && ((*argv + i) != NULL); ++i)
			fprintf(stdout, "Argument: %s\n", (*argv + i));
	}

	while (*(argv + i) != NULL)
	{
		if (strcmp(*(argv + i), "|") == 0)
			num_pipes++;

		else if (strcmp(*(argv + i), ">") == 0 || strcmp(*(argv + i), "<") == 0 || strcmp(*(argv + i), ">>") == 0)
			num_redirects++;

		i++;
	}

	// No pipes or redirects, just execute the command normally.
	// This is the most common case and the easiest to handle.
	// We just need to fork the process, and execute the command in the child process.
	if (num_pipes == 0 && num_redirects == 0)
	{
		pid = fork();

		if (pid < 0)
		{
			fprintf(stderr, "%s: %s\n", ERR_SYSCALL, strerror(errno));
			exit(EXIT_FAILURE);
		}

		// Child process, execute the command, and print an error if it fails.
		if (pid == 0)
		{
			// Restore the default signal handler for SIGINT.
			signal(SIGINT, SIG_DFL);

			// Execute the command.
			if (execvp(*argv, argv) == -1)
			{
				fprintf(stderr, "%s: %s\n", ERR_SYSCALL, strerror(errno));
				exit(EXIT_FAILURE);
			}

			// Exit the child process, since execvp replaces the child process with the command.
			exit(EXIT_SUCCESS);
		}

		// Parent process, wait for the child process to finish, 
		// as we don't support background processes (this is too advanced for this assignment).
		else
		{
			waitpid(pid, &status, 0);

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

	// Pipes and redirects handling.
	// We need to handle pipes and redirects separately, because we need to know the next argument.
	// This part was literally the hardest part of the assignment, because we couldn't figure out how to do it.
	// Oh well, it works now, WITH UNLIMITED PIPES AND REDIRECTS!
	else
	{
		char **args = argv;
		char *redirect_file;
		int pipe_fd[2];
		int input_fd = STDIN_FILENO, output_fd = STDOUT_FILENO, redirect_fd = -1;

		// Loop through all the arguments.
		// We need to loop through all the arguments, because we need to find the pipes and redirects.
		// We also need to find the end of the command, so we can execute it.
		// We can't just execute the command when we find a pipe or redirect, because we need to know the next argument.
		while (*args != NULL)
		{
			// Pipe handling.
			if (strcmp(*args, "|") == 0)
			{
				// Create a pipe.
				if (pipe(pipe_fd) == -1)
				{
					fprintf(stderr, "%s: %s\n", ERR_SYSCALL, strerror(errno));
					exit(EXIT_FAILURE);
				}

				// Execute the command.
				execute_command_piped(argv, input_fd, *(pipe_fd + 1));

				// Close the write end of the pipe.
				close(*(pipe_fd + 1));

				// Set the read end of the pipe as the input for the next command.
				input_fd = *pipe_fd;

				// Move to the next argument.
				argv = args + 1;
			}

			// Redirect handling.
			else if (strcmp(*args, ">") == 0 || strcmp(*args, "<") == 0 || strcmp(*args, ">>") == 0)
			{
				// Get the file name.
				redirect_file = *(args + 1);

				// Output redirection.
				if (strcmp(*args, ">") == 0)
					redirect_fd = open(redirect_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);

				// Input redirection.
				else if (strcmp(*args, "<") == 0)
					redirect_fd = open(redirect_file, O_RDONLY);

				// Append redirection.
				else if (strcmp(*args, ">>") == 0)
					redirect_fd = open(redirect_file, O_WRONLY | O_CREAT | O_APPEND, 0644);

				// Error handling.
				if (redirect_fd == -1)
				{
					fprintf(stderr, "%s: %s\n", ERR_SYSCALL, strerror(errno));
					exit(EXIT_FAILURE);
				}

				// Execute the command.
				execute_command_piped(argv, input_fd, redirect_fd);

				close(redirect_fd);

				argv = args + 2;
			}

			args++;
		}

		// Execute the last command.
		execute_command_piped(argv, input_fd, output_fd);
	}
}

void execute_command_piped(char **args, int input_fd, int output_fd) {
	pid_t pid;
	int status;

	pid = fork();

	if (pid < 0)
	{
		fprintf(stderr, "%s: %s", ERR_SYSCALL, strerror(errno));
		exit(EXIT_FAILURE);
	}

	// Child process, execute the command, and print an error if it fails.
	// The input and output file descriptors are passed as parameters.
	// The input file descriptor is used as the standard input, and the output file descriptor is used as the standard output.
	// The input and output file descriptors are closed after they are used.
	else if (pid == 0)
	{
		// Restore the default signal handler for SIGINT.
		signal(SIGINT, SIG_DFL);

		if (input_fd != STDIN_FILENO)
		{
			dup2(input_fd, STDIN_FILENO);
			close(input_fd);
		}

		if (output_fd != STDOUT_FILENO)
		{
			dup2(output_fd, STDOUT_FILENO);
			close(output_fd);
		}

		if (execvp(*args, args) == -1)
		{
			fprintf(stderr, "%s: %s", ERR_SYSCALL, strerror(errno));
			exit(EXIT_FAILURE);
		}
	} 
	
	// Parent process, wait for the child process to finish,
	// as we don't support background processes (this is too advanced for this assignment).
	else
	{
		waitpid(pid, &status, 0);

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