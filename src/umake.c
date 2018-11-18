/* CSCI 347 micro-make
 * 
 * 09 AUG 2017, Aran Clauson
 *
 * UPDATED:
 * 9 NOV 2018, Melody Grappo
 */


#include <unistd.h>
#include <sys/wait.h>
#include "arg_parse.h"
#include "target.h"
#include "list.h"
#include "processline.h"


/* CONSTANTS */

/* PROTOTYPES */

/* Main entry point.
 * argc    A count of command-line arguments 
 * argv    The command-line argument values
 *
 * Micro-make (umake) reads from the uMakefile in the current working
 * directory.  The file is read one line at a time. Lines are parsed into targets
 * with associated rules.  All targets and connected rules are parsed. Target's given
 * by command linehave their rules processed by processline.
 *
 * Assumes uMakefile follows standard formatting rules
 */
int main(int argc, const char* argv[]) {

    if (argc < 2) {
	printf("must name at least one target\n");
	printf("usage: %s <target_1> <target_2> ... <target_n>\n", argv[0]);
	exit(1);
    } 


     
    FILE*   makefile  = fopen("./uMakefile", "r");
    if (makefile == NULL) {
	perror("./uMakefile");
	exit(1);
    }

    size_t      bufsize   = 0;
    char*       line      = NULL;
    char*       buf       = NULL;
    ssize_t     linelen   = getline(&line, &bufsize, makefile);
    target*     t         = NULL;
    target_list l         = NULL;
    int         count     = 0;
    int         in_target = 0;

    while(-1 != linelen) {

        if(line[linelen-1]=='\n') {
            linelen -= 1;
            line[linelen] = '\0';
        }

        if (line[0] != '#') {	
	    if (is_target(line) && !in_target) {
		in_target = 1;

		buf = target_parsename(line);
		t = target_new(buf);
		free(buf);

		target_adddep(t, target_parsedep(line));

	    } else if (is_target(line) && in_target) {
		list_append(&l, (cons*) t);

		buf = target_parsename(line);
		t = target_new(buf);
		free(buf);
		
		target_adddep(t, target_parsedep(line));

	    } else if (in_target && line[0] == '\t') {
		
		target_addrule(t, line);


	    } else if (!is_target(line)) {
		defenv(line);
	    }
	}
        linelen = getline(&line, &bufsize, makefile);

    }

    list_append(&l, (cons*) t);

    count = 1;
    while (count < argc) {

	t = target_findmatch(l, argv[count]);
	processdep(l, t);
        	
	count++;
    }


    // free list
    target* tmp = NULL;
    while (l != NULL) {
	tmp = (target*) l;
	l = l->next;
	target_free(tmp);
    }

    fclose(makefile);
    free(line);

    return EXIT_SUCCESS;
}

