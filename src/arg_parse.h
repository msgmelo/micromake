#ifndef ARG_PARSE_H
#define ARG_PARSE_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

/* Argument Parse
 * line     The arguments to be parsed
 * argcp    The location to store the argument count
 *
 * This function returns an array of new pointers that point to the beginning
 * character of each word in a line, ignoring whitespace. The number of arguments
 * will be returned via the argcp parameter.
 */
char** arg_parse(char* line, int* argcp);

int arg_count(char* line);

void arg_copy(char* line, char** args);


#endif
