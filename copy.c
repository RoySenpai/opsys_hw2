/*
 *  Operation Systems (OSs) Course Assignment 2
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
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

/*
 * @brief The copy options.
 *
 * @note OPT_NONE - No options (default).
 * @note OPT_VERBOSE - Print the copied file.
 * @note OPT_FORCE - Force the copy even if the file already exists.
*/
typedef enum {
    OPT_NONE = 0,
    OPT_VERBOSE = 1,
    OPT_FORCE = 2
} copy_options;

/*
 * @brief Copy a file to another file.

 * @param f1 First file to copy (required).
 * @param f2 Second file to copy to (required).
 * 
 * @return 0 if the copy was successful.
 * @return 1 if an error occurred (e.g. file not found).
 * 
 * @note The user can provide the -v option to print the copied file.
 * @note The user can provide the -f option to force the copy even if the file already exists.
*/
int main(int argc, char** argv) {
    FILE *f1 = NULL, *f2 = NULL;

    copy_options options = OPT_NONE;

    char c = '\0';

    // Check if the user provided enough arguments (2 files), otherwise print the usage.
    // The options are optional.
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <source> <destination> [-vf]\n", argv[0]);
        return 1;
    }

    // Parse the options.
    for (int i = 3; i < argc; ++i)
    {
        if (strcmp(argv[i], "-v") == 0)
            options |= OPT_VERBOSE;

        else if (strcmp(argv[i], "-f") == 0)
            options |= OPT_FORCE;
        
        else
        {
            if (options & OPT_VERBOSE)
                fprintf(stderr, "general failure\n");
                
            return 1;
        }
    }

    f1 = fopen(argv[1], "r");

    if (f1 == NULL)
    {
        if (options & OPT_VERBOSE)
            fprintf(stderr, "general failure\n");

        return 1;
    }

    // Check if the file already exists (and the user didn't provide the -f option).
    if (!(options & OPT_FORCE))
    {
        f2 = fopen(argv[2], "r");

        if (f2 != NULL)
        {
            if (options & OPT_VERBOSE)
                fprintf(stderr, "target file exist\n");

            fclose(f2);

            return 1;
        }
    }

    f2 = fopen(argv[2], "w");

    if (f2 == NULL)
    {
        if (options & OPT_VERBOSE)
            fprintf(stderr, "general failure\n");
        
        return 1;
    }

    // Copy the file to the destination, byte by byte.
    while ((c = fgetc(f1)) != EOF)
        fputc(c, f2);

    fclose(f1);
    fclose(f2);

    if (options & OPT_VERBOSE)
        fprintf(stdout, "success\n");

    return 0;
}