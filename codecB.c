/*
 *  Operation Systems (OSs) Course Assignment 2
 *  Codec B shared library for encoding and decoding strings
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
#include "codec.h"

/*
 * @brief Encodes a string by adding 3 to each character (in ASCII).
 *
 * @param input The string to encode.
 * @param len The length of the string.
 * 
 * @return The encoded string or NULL if malloc failed.
 * 
 * @note The returned string must be freed by the caller.
*/
char* encode(char* input, size_t len) {
    char* output = (char*) malloc(len + 1);

    if (output == NULL)
        return NULL;

    for (size_t i = 0; i < len; ++i)
        *(output + i) = (*(input + i) + 0x03);

    // Add null terminator
    *(output + len) = '\0';

    return output;
}

/*
 * @brief Decodes a string by subtracting 3 from each character (in ASCII).
 *
 * @param input The string to decode.
 * @param len The length of the string.
 * 
 * @return The decoded string or NULL if malloc failed.
 * 
 * @note The returned string must be freed by the caller.
*/
char* decode(char* input, size_t len) {
    char* output = malloc(len + 1);

    if (output == NULL)
        return NULL;

    for (size_t i = 0; i < len; ++i)
        *(output + i) = (*(input + i) - 0x03);

    // Add null terminator
    *(output + len) = '\0';

    return output;
}