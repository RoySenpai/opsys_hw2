/*
 *  Operation Systems (OSs) Course Assignment 2
 *  Compare program
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
 * @brief Options for the cmp commands.
 *
 * @note OPT_NONE - no options (default).
 * @note OPT_IGNORE_CASE - ignore case when comparing.
 * @note OPT_VERBOSE - verbose output (print equal or distinct).
*/
typedef enum {
    OPT_NONE = 0,
    OPT_IGNORE_CASE = 1,
    OPT_VERBOSE = 2
} cmp_options;

/*
 * @brief Result of the comparison.
 *
 * @note EQUAL - The files are equal.
 * @note DISTINCT - The files are distinct, or an error occurred (e.g. file not found).
*/
typedef enum {
    EQUAL = 0,
    DISTINCT = 1
} cmp_result;

/*
 * @brief Compare two files and print the result.
 *
 * @param f1 First file to compare (required).
 * @param f2 Second file to compare (required).
 * @param options Options for the cmp command (optional).
 * 
 * @return 0 if the files are equal (not distinct).
 * @return 1 if they are distinct (not equal) or if an error occurred (e.g. file not found).
 * 
 * @note The user can provide the -i option to ignore case when comparing.
 * @note The user can provide the -v option to print the result (equal or distinct).
*/
int main(int argc, char** argv) {
    FILE *f1 = NULL, *f2 = NULL;

    cmp_options options = OPT_NONE;

    char c1 = '\0', c2 = '\0';

    bool equal = true;

    // Check if the user provided enough arguments (2 files), otherwise print the usage.
    // The options are optional.
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s file1 file2 [-iv]\n", *(argv));
        return (int)DISTINCT;
    }

    // Parse the options
    for (int i = 3; i < argc; ++i)
    {
        if (strcmp(*(argv + i), "-i") == 0)
            options |= OPT_IGNORE_CASE;
        
        else if (strcmp(*(argv + i), "-v") == 0)
            options |= OPT_VERBOSE;

        else
        {
            fprintf(stderr, "[CMP] Unknown option \"%s\".\n", *(argv + i));
            return (int)DISTINCT;
        }
    }

    f1 = fopen(*(argv + 1), "r");
    f2 = fopen(*(argv + 2), "r");

    if (f1 == NULL)
    {
        if (options & OPT_VERBOSE)
            fprintf(stderr, "[CMP] Error opening file \"%s\" for reading.\n", *(argv + 1));

        return (int)DISTINCT;
    }

    if (f2 == NULL)
    {
        if (options & OPT_VERBOSE)
            fprintf(stderr, "[CMP] Error opening file \"%s\" for reading.\n", *(argv + 2));
        
        return (int)DISTINCT;
    }

    // Check the files size
    fseek(f1, 0, SEEK_END);
    fseek(f2, 0, SEEK_END);

    // If the files are not the same size, they are distinct for sure
    if (ftell(f1) != ftell(f2))
    {
        if (options & OPT_VERBOSE)
            fprintf(stdout, "distinct\n");

        fclose(f1);
        fclose(f2);

        return (int)DISTINCT;
    }

    // Reset the file pointers
    fseek(f1, 0, SEEK_SET);
    fseek(f2, 0, SEEK_SET);

    // Compare the files byte by byte.
    while (equal)
    {
        c1 = fgetc(f1);
        c2 = fgetc(f2);

        if (c1 == EOF || c2 == EOF)
            break;

        if (options & OPT_IGNORE_CASE)
        {
            c1 = tolower(c1);
            c2 = tolower(c2);
        }

        if (c1 != c2)
            equal = false;
    }

    fclose(f1);
    fclose(f2);

    if (!equal)
    {
        if (options & OPT_VERBOSE)
            fprintf(stdout, "distinct\n");

        return (int)DISTINCT;
    }

    if (options & OPT_VERBOSE)
        fprintf(stdout, "equal\n");

    return (int)EQUAL;
}