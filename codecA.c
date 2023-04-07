/*
 *  Operation Systems (OSs) Course Assignment 2
 *  Codec A shared library for encoding and decoding strings
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
 * @brief Encodes a string by changing the case of each character.
 * 
 * @param input the string to encode.
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
    {
        char tmp = *(input + i);

        // To lower case
        if (tmp > 0x40 && tmp < 0x5B)
            tmp += 0x20;

        // To upper case
        else if (tmp > 0x60 && tmp < 0x7B)
            tmp -= 0x20;
        
        *(output + i) = tmp;
    }

    // Add null terminator
    *(output + len) = '\0';

    return output;
}

/*
 * @brief Decodes a string by changing the case of each character.
 * 
 * @param input The string to decode.
 * @param len The length of the string.
 * 
 * @return The decoded string or NULL if malloc failed.
 * 
 * @note The returned string must be freed by the caller.
 * @note Since the encoding and decoding are the same, this function 
 *          simply calls the encode function.
*/
char* decode(char* input, size_t len) {
    return encode(input, len);
}