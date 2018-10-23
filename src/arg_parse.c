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
 * character of each word in a line, ignoring whitespace.
 */
char** arg_parse(char* line, int* argcp)
{
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

    char** args = (char**) malloc((count + 1)*sizeof(char*));
    if (args == NULL) {
        perror("malloc");
        exit(1);
    }

    int j   = 0;
    i       = 0;
    in_word = 0;

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
	
    

    args[count] = NULL;
    *argcp = count; 
    
    return args;
}
