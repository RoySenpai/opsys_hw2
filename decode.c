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
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h> // for dlopen, dlsym, dlclose

/*
 * This function is used to decode a given text using a given codec.
 *
 * @note The function is loaded dynamically from a shared library.
 * 
 * @note The returned pointer must be freed by the caller.
 * 
 * @param text The text to decode.
 * 
 * @param len The length of the text.
 * 
 * @return Pointer to the decoded text.
 * 
 * @return NULL if an error occurred.
*/
char* (*decode)(char*, size_t);

bool isCodec(char* codec) {
    return strcmp(codec, "codecA") == 0 || strcmp(codec, "codecB") == 0;
}

int main(int argc, char** argv) {
    void* handle;
    char* decoded;
    size_t len;

    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <codec type> <text>\n", argv[0]);
        return 1;
    }

    if (!isCodec(argv[1]))
    {
        fprintf(stderr, "Error: codec type must be either codecA or codecB.\n");
        return 1;
    }

    if ((len = strlen(argv[2])) == 0)
    {
        fprintf(stderr, "Error: text must not be empty.\n");
        return 1;
    }

    handle = dlopen((strcmp(argv[1], "codecA") == 0 ? "./libcodecA.so":"./libcodecB.so"), RTLD_LAZY);

    if (!handle)
    {
        fprintf(stderr, "Error: %s\n", dlerror());
        return 1;
    }

    decode = dlsym(handle, "decode");

    if (decode == NULL)
    {
        fprintf(stderr, "Error: %s\n", dlerror());
        return 1;
    }

    decoded = decode(argv[2], len);

    if (decoded == NULL)
    {
        fprintf(stderr, "Fatal error\n");
        return 1;
    }

    fprintf(stdout, "%s\n", decoded);

    free(decoded);

    return 0;
}