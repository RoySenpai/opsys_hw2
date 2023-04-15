/*
 *  Operation Systems (OSs) Course Assignment 2
 *  Decoding program
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
 * @brief The decode result.
 * @note SUCCESS - The decode was successful.
 * @note FAILURE - An error occurred (e.g. library not found, wrong arguments).
*/
typedef enum {
    SUCCESS = 0,
    FAILURE = 1
} decode_result;

/*
 * @brief This function is used to decode a given text using a given codec.
 * @note The function is loaded dynamically from a shared library.
 * @note The returned pointer must be freed by the caller.
 * @param text The text to decode.
 * @param len The length of the text.
 * @return Pointer to the decoded text.
 * @return NULL if an error occurred.
*/
unsigned char* (*decode)(unsigned char*, size_t);

/*
 * @brief This function is used to check if a given codec is valid.
 * @param codec The codec to check.
 * @return true if the codec is valid.
 * @return false if the codec is invalid.
*/
bool isCodec(char* codec) {
    return strcmp(codec, "codecA") == 0 || strcmp(codec, "codecB") == 0;
}

/*
 * @brief Decodes a given text using a given codec and print the result to stdout.
 * @param codec The codec to be used.
 * @param text The text to decode.
 * @return 0 if the operation was successful.
 * @return 1 if an error occurred.
 * @note The program usage is: decode <codec type> <text>
 * @note The codec type must be either codecA or codecB (case senestive).
 * @note The text must not be empty.
 * @note The program will print an error message to stderr if an error occurred.
 * @note The program uses dynamic loading to load the decode function from a shared library.
*/
int main(int argc, char** argv) {
    void* handle = NULL;
    unsigned char* decoded = NULL;
    size_t len = 0;

    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <codec type> <text>\n", *(argv));
        return (int)FAILURE;
    }

    if (!isCodec(*(argv + 1)))
    {
        fprintf(stderr, "[Decoder] Error: codec type must be either codecA or codecB (case senestive).\n");
        return (int)FAILURE;
    }

    if (!(len = strlen(*(argv + 2))))
    {
        fprintf(stderr, "[Decoder] Error: text must not be empty.\n");
        return (int)FAILURE;
    }

    if ((handle = dlopen((strcmp(*(argv + 1), "codecA") == 0 ? "./libcodecA.so":"./libcodecB.so"), RTLD_LAZY)) == NULL)
    {
        fprintf(stderr, "[Decoder] Error: %s\n", dlerror());
        return (int)FAILURE;
    }

    if ((decode = dlsym(handle, "decode")) == NULL)
    {
        fprintf(stderr, "[Decoder] Error: %s\n", dlerror());
        return (int)FAILURE;
    }

    if ((decoded = decode((unsigned char*)*(argv + 2), len)) == NULL)
    {
        fprintf(stderr, "[Decoder] Error: Falied to malloc.\n");
        return (int)FAILURE;
    }

    fprintf(stdout, "%s\n", decoded);

    // Free resources and close the library handle (cleanup).
    dlclose(handle);
    free(decoded);

    return (int)SUCCESS;
}