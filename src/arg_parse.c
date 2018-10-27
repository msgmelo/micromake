/* CSCI 347 micro-make
 * UPDATED:
 * 12 OCT 2018, Melody Grappo
 */


#include "arg_parse.h"

/* CONSTANTS */

/* PROTOTYPES */

/* Argument Parse
 * line     The arguments to be parsed
 * argcp    The location to store the argument count
 *
 * This function returns an array of new pointers that point to the beginning
 * character of each word in a line, ignoring whitespace
 *
 * Operates in two parts, first part count the number of arguments, the second part
 * adds the location of the beginning of each argument to a new heap-allocated array,
 * which is returned.
 */
char** arg_parse(char* line, int* argcp)
{
    assert(argcp != NULL && "must pass valid pointer to arg_parse");

    int count = arg_count(line);

    char** args = halloc((count + 1)*sizeof(char*));

    arg_copy(line, args);
    *argcp = count; 
    return args;
}

/* Argument count
 * line    The arguments to be counted
 *
 * This function counts the number of arguments within a line.
 */
int arg_count(char* line) {


    int count    = 0;
    int i        = 0;
    int in_word  = 0; 
   
    while(line[i] != '\0') {
	
        if (isspace(line[i]) && in_word) {             
	    in_word    = 0;
        } else if (!isspace(line[i]) && !in_word) {
            in_word    = 1;
            count++;
        } 
	i++;
    }

    return count;
}


/* Argument copy
 * line    The original arguments
 * args    The pointer array to store the beginning of each argument
 * 
 * This function copies the location of the beginning of each argument
 * in a line into the args array, and null terminates each argument within
 * the line.
 */
void arg_copy(char* line, char** args){

    int i       = 0;
    int j       = 0;
    int in_word = 0;

    while(line[i] != '\0') {

	if (isspace(line[i]) && in_word) {
	    in_word = 0;
	    line[i] = '\0';
	} else if (!isspace(line[i]) && !in_word) {
	    in_word = 1;
	    args[j] = &line[i];
	    j++;
	}
	
	i++;
    }
    
    args[j] = NULL;
}