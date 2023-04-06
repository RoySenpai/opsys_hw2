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

#ifndef _CODEC_H
#define _CODEC_H

#include <stddef.h>

/*
 * Function to encode a string.
 * 
 * @note The function should be implemented in codecA.c or codecB.c.
 * 
 * @param input The string to encode.
 * 
 * @param len The length of the string.
 * 
 * @return The encoded string or NULL if malloc failed.
*/
char* encode(char* input, size_t len);

/*
 * Function to decode a string.
 * 
 * @note The function should be implemented in codecA.c or codecB.c.
 * 
 * @param input The string to decode.
 * 
 * @param len The length of the string.
 * 
 * @return The decoded string or NULL if malloc failed.
*/
char* decode(char* input, size_t len);

#endif