/*
 *  Operation Systems (OSs) Course Assignment 2
 *  Codec Header File 
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

#ifndef _CODEC_H
#define _CODEC_H

#include <stddef.h>

/*********************/
/* Predefined values */
/*********************/

/*
 * @brief The case value for the encoding/decoding.
 * @note The case value is used to determine if a character is upper or lower case.
 * @note Used in codecA.c.
*/
#define U_CHAR_CASE 0x20

/*
 * @brief The addition/subtraction value for the encoding/decoding.
 * @note The addition/subtraction value is used to determine if a character is upper or lower case.
 * @note Used in codecB.c.
*/
#define U_CHAR_ADDITION 0x03

/* 
 * @brief The modulus value for the encoding/decoding.
 * @note The modulus value is used to avoid overflow.
 * @note Used in codecB.c.
*/
#define U_CHAR_MODULUS 0xFF


/*************************/
/* Function declerations */
/*************************/

/*
 * @brief Function to encode a string.
 * @param input The string to encode.
 * @param len The length of the string.
 * @return The encoded string or NULL if malloc failed.
 * @note The function should be implemented in codecA.c and codecB.c.
 * @note The function shouldn't change the original string.
 * @note The function shouldn't be implemented in encode.c/decode.c.
*/
unsigned char* encode(unsigned char* input, size_t len);

/*
 * @brief Function to decode a string.
 * @param input The string to decode.
 * @param len The length of the string.
 * @return The decoded string or NULL if malloc failed.
 * @note The function should be implemented in codecA.c and codecB.c.
 * @note The function shouldn't change the original string.
 * @note The function shouldn't be implemented in encode.c/decode.c.
*/
unsigned char* decode(unsigned char* input, size_t len);

#endif