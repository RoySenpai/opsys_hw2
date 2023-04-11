/*
 *  Operation Systems (OSs) Course Assignment 2
 *  Shell program header file
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

#ifndef _STSHELL_H
#define _STSHELL_H

/***********************************/
/* Internal Settings for the shell */
/***********************************/

/*
 * @brief Toggle debug mode.
 *
 * @note Used to toggle debug mode, which prints debug messages.
 * @note 0 - Debug mode is off.
 * @note 1 - Debug mode is on.
*/
#define DEBUG_MODE 0

/*
 * @brief Maximum command length.
 *
 * @note Commands longer than this will be truncated.
 * @note The default value is 1024 characters.
 */
#define MAX_COMMAND_LENGTH 1024

/*
 * @brief Maximum number of arguments.
 *
 * @note More than this number of arguments will be truncated.
 * @note The default value is 10.
 */
#define MAX_ARGS 10

/*
 * @brief Maximum path length.
 *
 * @note Paths longer than this will be truncated.
 * @note The default value is 1024 characters.
 */
#define MAX_PATH_LENGTH 1024


/*******************/
/* Program details */
/*******************/

/*
 * @brief Program license text.
*/
#define CMD_GNU_LICENSE "This program is free software: you can redistribute it and/or modify\n" \
                        "it under the terms of the GNU General Public License as published by\n" \
                        "the Free Software Foundation, either version 3 of the License, or\n" \
                        "(at your option) any later version.\n" \
                        "This program is distributed in the hope that it will be useful,\n" \
                        "but WITHOUT ANY WARRANTY; without even the implied warranty of\n" \
                        "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n" \
                        "GNU General Public License for more details.\n" \
                        "You should have received a copy of the GNU General Public License\n" \
                        "along with this program.  If not, see <https://www.gnu.org/licenses/>.\n"

/******************/
/* Error Messages */
/******************/

/*
 * @brief System call failure error message.
 *
 * @note Used to indicate a system call failure, and print the error. 
*/
#define ERR_SYSCALL "System call faliure"

/* 
 * @brief Change directory error message: no such file or directory.
 *
 * @note Used to indicate a change directory failure, and print the error.
*/
#define ERR_CMD_CD "cd: No such file or directory"

/*
 * @brief Change directory error message: too many arguments.
 *
 * @note Used to indicate that the used provided too many arguments to the cd command.
*/
#define ERR_CMD_CD_ARG "cd: Too many arguments"

/*
 * @brief Command not found error message.
 *
 * @note Used to indicate that the command was not found.
*/
#define ERR_CMD_NOT_FOUND "Command not found"


/*******************/
/* Command Aliases */
/*******************/

/*
 * @brief Alias for the exit command.
 *
 * @note Used to indicate that the user wants to exit the shell.
 * @note This is the only internal command required by the assignment.
*/
#define CMD_EXIT "exit"

/*
 * @brief Alias for the cd command.
 *
 * @note Used to indicate that the user wants to change the current working directory.
 * @note This is a custom made command and is not part of the assignment.
*/
#define CMD_CD "cd"

/*
 * @brief Alias for the clear command.
 *
 * @note Used to indicate that the user wants to clear the screen.
 * @note This is a custom made command and is not part of the assignment.
*/
#define CMD_CLEAR "clear"

/*
 * @brief Alias for the pwd command.
 *
 * @note Used to indicate that the user wants to print the current working directory.
 * @note This is a custom made command and is not part of the assignment.
*/
#define CMD_PWD "pwd"

/*
 * @brief Print the license of the shell.
 *
 * @note This is a custom made command and is not part of the assignment.
*/
#define CMD_GNU "gnu"


/**********************/
/* Clean screen stuff */
/**********************/

/*
 * @brief Clear and flush the screen.
 *
 * @note Used to clear the screen and flush the output buffer.
*/
#define CMD_CLEAR_FLUSH "\33[H\33[2J"

/*
 * @brief Clear and flush the screen, command length.
 *
 * @note Indicates the length of the clear and flush command.
*/
#define CMD_CLEAR_FLUSH_LEN 7


/****************/
/* Enumerations */
/****************/

/* 
 * @brief Command type enum.
 *
 * @note Used to indicate whether a command is internal or external.
*/
typedef enum _CommandType
{
    Internal = 0,
    External = 1
} CommandType;

/*
 * @brief Result enum.
 *
 * @note Used to indicate success or failure of an internal command.
*/
typedef enum _Result
{
    Success = 0,
    Failure = 1
} Result;


/*************************/
/* Function declarations */
/*************************/

/*
 * @brief Signal handler for SIGINT.
 *
 * @param sig The signal number.
 *
 * @return void (nothing).
 *
 * @note This function does absolutely nothing,
 * 	 		and is used to ignore SIGINT.
 */
void sigint_handler(int sig);

/*
 * @brief Parse a command into arguments.
 * 
 * @param command The command to parse.
 * @param argv The array of arguments.
 * 
 * @return 0 if it an internal command, 1 if it is an external command.
 * 
 * @note This function will modify the argv array.
 * @note This function will execute internal commands.
*/
CommandType parse_command(char* command, char** argv);

/*
 * @brief Execute a command.
 *
 * @param argv The array of arguments.
 *
 * @return void (nothing).
 */
void execute_command(char** argv);

/*
 * @brief Execute change directory command.
 *
 * @param path The path to change to.
 * @param argc The number of arguments.
 * 
 * @return Success if the command succeeded, Failure otherwise.
 * @note number of arguments must be 1.
*/
Result cmdCD(char *path, int argc);



#endif