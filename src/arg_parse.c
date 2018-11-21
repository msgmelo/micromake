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
   
    while(line[i] != '\0' && line[i] != '#') {
	
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

    while(line[i] != '\0' && line[i] != '#') {

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

char** arg_trunc(char** args, int* argcp, int loc) {
    int ct = *argcp - 2;
    int i = 0;
    int j = 0;
    char** update = halloc(sizeof(char*) * ct);
    while (j < ct && i < *argcp) {

	if (i != loc && i != (loc+1)) {
	    update[j] = args[i];
	    j++;
	}
	i++;
    }

    free(args);
    *argcp = ct;

    return update;
}

int arg_isIO(char* c) {
    if (strcmp(c, ">") == 0)
	return ARG_T;
    else if (strcmp(c, "<") == 0)
	return ARG_R;
    else if (strcmp(c, ">>") == 0)
	return ARG_A;
    else
	return -1;
}


int arg_containsIO(char** argv, int argc) {
    int i = 0;
    int loc = -1;
    int found = 0;
    while ( i < argc && found == 0) {
	if (arg_isIO(argv[i]) != -1){
	    loc = i;
	    found = 1;
	}
	i++;
    }

    return loc;
}

char** arg_IOred(char** argv, int argcp) {
    int loc = arg_containsIO(argv, argcp);
    if (loc == -1) {
	return argv;
    } else {
	int fd;
	switch (arg_isIO(argv[loc])) {
	    case ARG_T: 
		fd = open(argv[loc+1], O_WRONLY | O_CREAT | O_TRUNC, 0600);
		dup2(fd, 1);
		break;
	    
	    case ARG_R:
		fd = open(argv[loc+1], O_RDONLY, 0600);
		if (fd == -1) {
		    perror("open");
		    abort();
		}
		dup2(fd, 0);
		break;

	    case ARG_A:
		fd = open(argv[loc+1], O_RDWR | O_CREAT | O_APPEND, 0600);
		dup2(fd, 1);
		break;
	}
        char** new = arg_trunc(argv, &argcp, loc);
	return arg_IOred(new, argcp);
   }
}
