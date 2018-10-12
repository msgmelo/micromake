/* CSCI 347 arg_parse
 
 * 12 OCT 2018, Melody Grappo
 */


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "arg_parse.h"

/* CONSTANTS */
#define MAX_ARGS 24 // maximum amount of arguments for the executing function

/* PROTOTYPES */

/* Argument Parse
 * This function returns an array of new pointers that point to the beginning
 * character of each word in a line, ignoring whitespace.
 */
char** arg_parse(char* line)
{
    int count = 0;
    int i     = 0;
    int word  = 0; // stores whehter iterator is currently within a word
    int tmp[MAX_ARGS]; // stores the locations beginnings of the words
   
    // count number of words in line
    while(line[i] != '\0') {
	
        if (isspace(line[i]) && word) { // just finished a word
            word    = 0;
            line[i] = '\0'; // null terminate the word
        } else if (!isspace(line[i]) && !word) {
            word       = 1;
            tmp[count] = i;
            count++;
        } // end if-else

	i++;

    } // end while

    // allocate space for args, adding 1 to count for null pointer
    char** args = (char**) malloc((count + 1)*sizeof(char*));
    if (args == NULL) {
        perror("malloc");
        exit(1);
    }

    // initalize malloc'd array with the chars in line indicated by
    // the locations in the tmp array
    int loc = 0;
    for (i = 0; i < count; i++) {
        loc     = tmp[i];
        args[i] = &line[loc];
    }

    args[count] = '\0'; // null terminate args

    return args;
}
