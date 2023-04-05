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

#include <argp.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const char *argp_program_version = "Compare v1.0";

const char *argp_program_bug_address = "me@hiyorix.com";

char doc[] = "The tool will compare two files, and return \"0\" if they are equal, and \"1\" if not.";

char args_doc[] = "[FILENAME]...";

struct argp_option options[] = { 
    { "verbose", 'v', 0, 0, "Verbose output: the tool will print \"equal\" or \"distinct\", in addition to returning the int value."},
    { "nocase", 'i', 0, 0, "Compare case insensitive instead of case sensitive."},
    { 0 } 
};

struct arguments {
    enum { NORMAL_MODE, VERBOSE_MODE } mode;
    bool isCaseInsensitive;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;

    switch (key)
    {
        case 'v': 
        {
            arguments->mode = VERBOSE_MODE;
            break;
        }

        case 'i': 
        {
            arguments->isCaseInsensitive = true;
            break;
        }

        case ARGP_KEY_ARG: return 0;

        default: return ARGP_ERR_UNKNOWN;
    }   

    return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

int main(int argc, char** argv) {
    struct arguments arguments;

    FILE *file1 = NULL, *file2 = NULL;

    char c1 = '\0', c2 = '\0';
    bool isDifferent = false;

    arguments.mode = NORMAL_MODE;
    arguments.isCaseInsensitive = false;

    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    if (argc < 2) {
        fprintf(stderr, "Error: Missing arguments. Please provide two files to compare.\n");
        return 1;
    }

    file1 = fopen(argv[3], "r");
    file2 = fopen(argv[4], "r");

    if (file1 == NULL || file2 == NULL)
    {
        fprintf(stderr, "Error: One or more files are missing.\n");
        return -1;
    }

    while (true)
    {
        c1 = fgetc(file1);
        c2 = fgetc(file2);

        if (arguments.isCaseInsensitive)
        {
            c1 = tolower(c1);
            c2 = tolower(c2);
        }

        if (c1 != c2)
        {
            isDifferent = true;
            break;
        }

        if (c1 == EOF || c2 == EOF)
        {
            break;
        }
    }

    fclose(file1);
    fclose(file2);

    if (arguments.mode == VERBOSE_MODE)
        printf("%s\n", isDifferent ? "distinct" : "equal");

    return isDifferent;
}