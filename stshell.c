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
#include <stdbool.h>
#include <fcntl.h>
#include <errno.h>
#include <strings.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/utsname.h>
#include "stshell.h"

char *homedir = NULL;
char *workingdir = NULL;
char *cwd = NULL;

int main(void) {
	char **argv = NULL;
	char command[MAX_COMMAND_LENGTH + 1] = { 0 };
	struct utsname sysinfo;

	if (uname(&sysinfo) == -1)
	{
        fprintf(stderr, "%s: uname error (%s)\n", ERR_SYSCALL, strerror(errno));
		return EXIT_FAILURE;
	}

	cwd = (char*) calloc((MAX_PATH_LENGTH + 1), sizeof(char));
	workingdir = (char*) calloc((MAX_PATH_LENGTH + 1), sizeof(char));
	argv = (char**) calloc((MAX_ARGS + 1), sizeof(char*));

	if (cwd == NULL || workingdir == NULL || argv == NULL)
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
				free(workingdir);

				return EXIT_FAILURE;
			}
		}

		// Prepare to print beutiful prompt.
		char* user_name = getenv("USER");
		char* cwd_changed = (char*) calloc((MAX_PATH_LENGTH + 1), sizeof(char));

		if (cwd_changed == NULL)
		{
			fprintf(stderr, "%s: calloc error (%s)\n", ERR_SYSCALL, strerror(errno));

			// Memory cleanup in case of malloc error.
			for (size_t i = 0; i < MAX_ARGS; ++i)
				free(*(argv + i));

			free(argv);
			free(cwd);
			free(cwd_changed);

			return EXIT_FAILURE;
		}

		strcpy(cwd_changed + 1, cwd + strlen(homedir));
		*cwd_changed = '~';

		// Print the prompt.
		fprintf(stdout, "%s@%s:%s%s", user_name, sysinfo.nodename, (strstr(cwd, homedir) == NULL ? cwd:cwd_changed), "$ ");
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
		{
			if (*(argv + i) != NULL)
				free(*(argv + i));
		}

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

	// Parse the first argument (the command itself).
	token = strtok(command, " ");

	// Exit command.
	if (strcmp(token, CMD_EXIT) == 0)
	{
		// Free the memory allocated for the arguments.
		for (size_t i = 0; i < MAX_ARGS; ++i)
		{
			if (*(argv + i) != NULL)
				free(*(argv + i));
		}

		// Free the memory allocated for the arguments array.
		free(argv);

		// Free the memory allocated for the current working directory.
		free(cwd);

		// Free the memory allocated for the working directory (privous directory, before changing to the current one).
		free(workingdir);

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
		if (*token == '"' && strlen(token) > 1)
		{
			char* tmp = (char*)calloc((MAX_ARG_LENGTH + 1), sizeof(char));

			if (tmp == NULL)
			{
				fprintf(stderr, "%s: %s\n", ERR_SYSCALL, strerror(errno));
				return Internal;
			}

			strcpy(tmp, (token + 1));

			while (token != NULL)
			{
				token = strtok(NULL, "\"");

				if (token != NULL)
				{
					strcat(tmp, " ");
					strcat(tmp, token);
				}
			}

			if (*(tmp + strlen(tmp) - 1) == '"')
				*(tmp + strlen(tmp) - 1) = '\0';

			memcpy(*pargv, tmp, strlen(tmp));
			++pargv;
			token = strtok(NULL, " ");

			free(tmp);

			continue;
		}

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

			getcwd(workingdir, MAX_PATH_LENGTH);

			return Success;
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
	int status, i = 0, num_pipes = 0;
	bool redirect = false;

	if (DEBUG_MODE)
	{
		fprintf(stdout, "Executing external command: %s\n", *argv);

		for (int i = 1; i < MAX_ARGS && (*(argv + i) != NULL); ++i)
			fprintf(stdout, "Argument %d: %s\n", i, *(argv + i));
	}

    while (*(argv + i) != NULL)
	{
        if (strcmp(*(argv + i), "|") == 0)
            num_pipes++;

		else if (strcmp(*(argv + i), ">") == 0 || strcmp(*(argv + i), ">>") == 0 || strcmp(*(argv + i), "<") == 0)
			redirect = true;

		i++;
    }

	// No pipes or redirects, just execute the command normally.
	// This is the most common case and the easiest to handle.
	if (num_pipes == 0 && !redirect)
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

			if (execvp(*argv, argv) == -1)
			{
				fprintf(stderr, "%s: %s\n", ERR_SYSCALL, strerror(errno));
				exit(EXIT_FAILURE);
			}

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

	// We have pipes and/or redirects, so we need to handle them.
	// We only support one redirect of each type, and at most two pipes.
	// This is the hardest case to handle.
	// We tried our best to make the code as readable as possible, but it's still quite complex.
	// Good luck reading it ;)
	else
	{
		pid = fork();

		if (pid < 0)
		{
			fprintf(stderr, "%s: %s\n", ERR_SYSCALL, strerror(errno));
			exit(EXIT_FAILURE);
		}

		// Child process, handle the pipes and redirects.
		else if (pid == 0)
		{
			char** argv_cpy = (char**)calloc((MAX_ARGS + 1), sizeof(char*));
			int input_fd = 0, output_fd = 1, append_fd = 1; // Input and output file descriptors.

			// Copy of argv, so we can modify it without affecting the original.
			if (argv_cpy == NULL)
			{
				fprintf(stderr, "%s: %s\n", ERR_SYSCALL, strerror(errno));
				exit(EXIT_FAILURE);
			}

			memcpy(argv_cpy, argv, (MAX_ARGS + 1) * sizeof(char*));

			// Restore the default signal handler for SIGINT.
			signal(SIGINT, SIG_DFL);

			// Only a redirection, no pipes.
			if (num_pipes == 0 && redirect)
			{
				bool in_mode = false, out_mode = false;

				for (i = 0; (i < MAX_ARGS) && (*(argv + i) != NULL) && !(in_mode && out_mode); ++i)
				{
					if (strcmp(*(argv + i), "<") == 0)
					{
						if (in_mode)
						{
							fprintf(stderr, "%s\n", ERR_REDIRECT_IN_TWICE);
							exit(EXIT_FAILURE);
						}

						input_fd = open(*(argv + i + 1), O_RDONLY);

						dup2(input_fd, STDIN_FILENO);
						close(input_fd);

						*(argv_cpy + i) = NULL;

						in_mode = true;
					}

					else if (strcmp(*(argv + i), ">") == 0)
					{
						if (out_mode)
						{
							fprintf(stderr, "%s\n", ERR_REDIRECT_OUT_TWICE);
							exit(EXIT_FAILURE);
						}

						output_fd = open(*(argv + i + 1), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

						dup2(output_fd, STDOUT_FILENO);
						close(output_fd);

						*(argv_cpy + i) = NULL;

						out_mode = true;
					}

					else if (strcmp(*(argv + i), ">>") == 0)
					{
						if (out_mode)
						{
							fprintf(stderr, "%s\n", ERR_REDIRECT_OUT_TWICE);
							exit(EXIT_FAILURE);
						}
						
						append_fd = open(*(argv + i + 1), O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);

						dup2(append_fd, STDOUT_FILENO);
						close(append_fd);

						*(argv_cpy + i) = NULL;

						out_mode = true;
					}

					i++;
				}

				if (execvp(*argv_cpy, argv_cpy) == -1)
				{
					fprintf(stderr, "%s: %s\n", ERR_SYSCALL, strerror(errno));
					exit(EXIT_FAILURE);
				}

				exit(EXIT_SUCCESS);
			}

			else if (num_pipes == 1)
			{
				int pipe_fd[2]; // Pipe file descriptors.

				if (pipe(pipe_fd) == -1)
				{
					fprintf(stderr, "%s: %s\n", ERR_SYSCALL, strerror(errno));
					exit(EXIT_FAILURE);
				}

				for (i = 0; i < MAX_ARGS && (*(argv + i) != NULL); ++i)
				{
					if (strcmp(*(argv + i), "|") == 0)
					{
						*(argv_cpy + i) = NULL;
						break;
					}
				}

				pid = fork();

				if (pid < 0)
				{
					fprintf(stderr, "%s: %s\n", ERR_SYSCALL, strerror(errno));
					exit(EXIT_FAILURE);
				}

				// Child process, handle the first command.
				else if (pid == 0)
				{
					dup2(*(pipe_fd + 1), STDOUT_FILENO);
					close(*pipe_fd);
					close(*(pipe_fd + 1));

					// Redirect input if needed.
					for (int j = 0; j < i; ++j)
					{
						if (strcmp(*(argv + j), "<") == 0)
						{
							input_fd = open(*(argv + j + 1), O_RDONLY);

							dup2(input_fd, STDIN_FILENO);
							close(input_fd);

							*(argv_cpy + j) = NULL;

							break;
						}

						else if (strcmp(*(argv + j), ">") == 0 || strcmp(*(argv + j), ">>") == 0)
						{
							fprintf(stderr, "%s\n", ERR_REDIRECT_OUT_IN_FIRST_PIPE);
							exit(EXIT_FAILURE);
						}
					}

					if (execvp(*argv_cpy, argv_cpy) == -1)
					{
						fprintf(stderr, "%s: %s\n", ERR_SYSCALL, strerror(errno));
						exit(EXIT_FAILURE);
					}

					exit(EXIT_SUCCESS);
				}

				// Parent process, handle the second command.
				else
				{
					dup2(*pipe_fd, STDIN_FILENO);
					close(*pipe_fd);
					close(*(pipe_fd + 1));

					// Redirect output if needed.
					for (int j = i + 1; j < MAX_ARGS && (*(argv + j) != NULL); ++j)
					{
						if (strcmp(*(argv + j), "<") == 0)
						{
							fprintf(stderr, "%s\n", ERR_REDIRECT_IN_IN_LAST_PIPE);
							exit(EXIT_FAILURE);
						}

						else if (strcmp(*(argv + j), ">") == 0)
						{
							output_fd = open(*(argv + j + 1), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

							dup2(output_fd, STDOUT_FILENO);
							close(output_fd);

							*(argv_cpy + j) = NULL;

							break;
						}

						else if (strcmp(*(argv + j), ">>") == 0)
						{
							append_fd = open(*(argv + j + 1), O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);

							dup2(append_fd, STDOUT_FILENO);
							close(append_fd);

							*(argv_cpy + j) = NULL;

							break;
						}
					}

					if (execvp(*(argv_cpy + i + 1), (argv_cpy + i + 1)) == -1)
					{
						fprintf(stderr, "%s: %s\n", ERR_SYSCALL, strerror(errno));
						exit(EXIT_FAILURE);
					}

					exit(EXIT_SUCCESS);
				}
			}

			else if (num_pipes == 2)
			{
				int pipe_fd[4]; // Pipe file descriptors.

				if (pipe(pipe_fd) == -1 || pipe(pipe_fd + 2) == -1)
				{
					fprintf(stderr, "%s: %s\n", ERR_SYSCALL, strerror(errno));
					exit(EXIT_FAILURE);
				}

				int loc_pipe1 = 0, loc_pipe2 = 0; // Location of the first and second pipe.

				for (i = 0; i < MAX_ARGS && (*(argv + i) != NULL); ++i)
				{
					if (strcmp(*(argv + i), "|") == 0)
					{
						*(argv_cpy + i) = NULL;

						if (loc_pipe1 == 0)
							loc_pipe1 = i;

						else
						{
							loc_pipe2 = i;
							break;
						}
					}
				}

				pid = fork();

				if (pid < 0)
				{
					fprintf(stderr, "%s: %s\n", ERR_SYSCALL, strerror(errno));
					exit(EXIT_FAILURE);
				}

				// Child process, handle the first command.
				else if (pid == 0)
				{
					dup2(*(pipe_fd + 1), STDOUT_FILENO);
					close(*pipe_fd);
					close(*(pipe_fd + 1));
					close(*(pipe_fd + 2));
					close(*(pipe_fd + 3));

					// Redirect input if needed.
					for (int j = 0; j < loc_pipe1; ++j)
					{
						if (strcmp(*(argv + j), "<") == 0)
						{
							input_fd = open(*(argv + j + 1), O_RDONLY);

							dup2(input_fd, STDIN_FILENO);
							close(input_fd);

							*(argv_cpy + j) = NULL;

							break;
						}

						else if (strcmp(*(argv + j), ">") == 0 || strcmp(*(argv + j), ">>") == 0)
						{
							fprintf(stderr, "%s\n", ERR_REDIRECT_OUT_IN_FIRST_PIPE);
							exit(EXIT_FAILURE);
						}
					}

					if (execvp(*argv_cpy, argv_cpy) == -1)
					{
						fprintf(stderr, "%s: %s\n", ERR_SYSCALL, strerror(errno));
						exit(EXIT_FAILURE);
					}

					exit(EXIT_SUCCESS);
				}

				pid = fork();

				if (pid < 0)
				{
					fprintf(stderr, "%s: %s\n", ERR_SYSCALL, strerror(errno));
					exit(EXIT_FAILURE);
				}

				// Child process, handle the second command.
				else if (pid == 0)
				{
					dup2(*pipe_fd, STDIN_FILENO);
					dup2(*(pipe_fd + 3), STDOUT_FILENO);
					close(*pipe_fd);
					close(*(pipe_fd + 1));
					close(*(pipe_fd + 2));
					close(*(pipe_fd + 3));

					for (int j = loc_pipe1 + 1; j < loc_pipe2; ++j)
					{
						if (strcmp(*(argv + j), "<") == 0 || strcmp(*(argv + j), ">") == 0 || strcmp(*(argv + j), ">>") == 0)
						{
							fprintf(stderr, "%s\n", ERR_REDIRECT_BETWEEN_PIPES);
							exit(EXIT_FAILURE);
						}
					}

					if (execvp(*(argv_cpy + loc_pipe1 + 1), (argv_cpy + loc_pipe1 + 1)) == -1)
					{
						fprintf(stderr, "%s: %s\n", ERR_SYSCALL, strerror(errno));
						exit(EXIT_FAILURE);
					}

					exit(EXIT_SUCCESS);
				}

				// Parent process, handle the third command.
				else
				{
					dup2(*(pipe_fd + 2), STDIN_FILENO);
					close(*pipe_fd);
					close(*(pipe_fd + 1));
					close(*(pipe_fd + 2));
					close(*(pipe_fd + 3));

					for (int j = loc_pipe2 + 1; j < MAX_ARGS && (*(argv + j) != NULL); ++j)
					{
						if (strcmp(*(argv + j), "<") == 0)
						{
							fprintf(stderr, "%s\n", ERR_REDIRECT_IN_IN_LAST_PIPE);
							exit(EXIT_FAILURE);
						}

						else if (strcmp(*(argv + j), ">") == 0)
						{
							output_fd = open(*(argv + j + 1), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

							dup2(output_fd, STDOUT_FILENO);
							close(output_fd);

							*(argv_cpy + j) = NULL;

							break;
						}

						else if (strcmp(*(argv + j), ">>") == 0)
						{
							append_fd = open(*(argv + j + 1), O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);

							dup2(append_fd, STDOUT_FILENO);
							close(append_fd);

							*(argv_cpy + j) = NULL;

							break;
						}
					}

					if (execvp(*(argv_cpy + loc_pipe2 + 1), (argv_cpy + loc_pipe2 + 1)) == -1)
					{
						fprintf(stderr, "%s: %s\n", ERR_SYSCALL, strerror(errno));
						exit(EXIT_FAILURE);
					}
				}
			}

			else
			{
				fprintf(stderr, "%s\n", ERR_TOO_MANY_PIPE);
				exit(EXIT_FAILURE);
			}
		}

		// Wait for all pipes, redirection and commands to finish.
		// This can't support background processes, and never will, as it's too advanced for this assignment.
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
}