#ifndef ARG_PARSE_H
#define ARG_PARSE_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include "list.h"

/* Argument Parse
 * line     The arguments to be parsed
 * argcp    The location to store the argument count
 *
 * This function returns an array of new pointers that point to the beginning
 * character of each word in a line, ignoring whitespace. The number of arguments
 * will be returned via the argcp parameter.
 */
char** arg_parse(char* line, int* argcp);


/* Argument Count
 * line     The arguments to be counted
 *
 * This function returns the number of arguments, or discrete words, in a given
 * line, and returns that count.
 */
int arg_count(char* line);


/* Argument Copy
 * line      The original arguments
 * args      The pointer to the array to store the beginning of each argument
 *
 * This function copies the starting character of each argument into args.
 */
void arg_copy(char* line, char** args);

#endif
