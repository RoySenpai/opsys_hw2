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
 * @brief This function is used to decode a given text using a given codec.
 *
 * @note The function is loaded dynamically from a shared library.
 * @note The returned pointer must be freed by the caller.
 * 
 * @param text The text to decode.
 * @param len The length of the text.
 * 
 * @return Pointer to the decoded text.
 * @return NULL if an error occurred.
*/
char* (*decode)(char*, size_t);

/*
 * @brief This function is used to check if a given codec is valid.
 * 
 * @param codec The codec to check.
 * 
 * @return true if the codec is valid.
 * @return false if the codec is invalid.
*/
bool isCodec(char* codec) {
    return strcmp(codec, "codecA") == 0 || strcmp(codec, "codecB") == 0;
}

/*
 * @brief Decodes a given text using a given codec and print the result to stdout.
 *
 * @param codec The codec to be used.
 * @param text The text to decode.
 * 
 * @return 0 if the operation was successful.
 * @return 1 if an error occurred.
 * 
 * @note The program usage is: decode <codec type> <text>
 * @note The codec type must be either codecA or codecB (case senestive).
 * @note The text must not be empty.
 * @note The program will print an error message to stderr if an error occurred.
 * @note The program uses dynamic loading to load the decode function from a shared library.
*/
int main(int argc, char** argv) {
    void* handle = NULL;
    char* decoded = NULL;
    size_t len = 0;

    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <codec type> <text>\n", argv[0]);
        return 1;
    }

    if (!isCodec(argv[1]))
    {
        fprintf(stderr, "[Decoder] Error: codec type must be either codecA or codecB (case senestive).\n");
        return 1;
    }

    if (!(len = strlen(argv[2])))
    {
        fprintf(stderr, "[Decoder] Error: text must not be empty.\n");
        return 1;
    }

    if ((handle = dlopen((strcmp(argv[1], "codecA") == 0 ? "./libcodecA.so":"./libcodecB.so"), RTLD_LAZY)) == NULL)
    {
        fprintf(stderr, "[Decoder] Error: %s\n", dlerror());
        return 1;
    }

    if ((decode = dlsym(handle, "decode")) == NULL)
    {
        fprintf(stderr, "[Decoder] Error: %s\n", dlerror());
        return 1;
    }

    if ((decoded = decode(argv[2], len)) == NULL)
    {
        fprintf(stderr, "[Decoder] Error: Falied to malloc.\n");
        return 1;
    }

    fprintf(stdout, "%s\n", decoded);

    // Free resources and close the library handle (cleanup).
    dlclose(handle);
    free(decoded);

    return 0;
}