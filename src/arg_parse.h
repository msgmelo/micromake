#ifndef ARG_PARSE_H
#define ARG_PARSE_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "list.h"

#define ARG_T 1
#define ARG_R 2
#define ARG_A 3


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

/* Argument Truncate
 * argv     The orignial arguments
 * argcp    The original argument count
 * loc      The location at which the truncated arguments will terminate
 *
 * This function truncates the given arguments up to but not including the loc
 * returns truncated arguments
 */
char** arg_trunc(char** argv, int argcp, int loc);

/* Argument is IO
 * s      The string to determine whether it is an IO redirect symbol or not.
 *
 * returns -1     if s is not a redirection symbol
 * returns ARG_T  if s is the truncate symbol (>)
 * returns ARG_R  if s is the redirect symbol (<)
 * returns ARG_A  if s is the append symbol   (>>)
 */
int arg_isIO(char* s);

/* Argument Contains IO
 * argv     The command line to check for IO redirect
 * argc     The size of the command line
 * 
 * returns the location of the redirect symbol, 
 *         or -1 if no such symbol is found 
 */
int arg_containsIO(char** argv, int argcp);

/* Argument IO Redirect
 * argv    the command line that contains a redirect symbol
 * argc    the number of arguments in argv
 * loc     the location of the redirect symbol in argv
 *
 * This function performs the necessary file management to perfrom file IO 
 * redirect.
 * returns updated command list
 */
char** arg_IOred(char** argv, int argcp, int loc);
#endif
