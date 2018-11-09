/* CSCI 347 micro-make
 * 
 * 09 AUG 2017, Aran Clauson
 *
 * UPDATED:
 * 12 OCT 2018, Melody Grappo
 */


#include <unistd.h>
#include <sys/wait.h>
#include "arg_parse.h"
#include "target.h"
#include "list.h"


/* CONSTANTS */

/* PROTOTYPES */

/* Process Line
 * line   The command line to execute.
 * This function creates a new child process to execute the line and 
 * waits for that process to complete. 
 */
void processline(char* line);


/* Process Target
 * t     The target to process
 * This function calls processlin() for each rule in the given target
 */
void processtarget(target* t);


/* Process Dependencies
 * l     The list of targets
 * t     The target whose dependcies will be processed
 * This recursive function  calls processtarget on each of t's dependencies.
 */
void processdep (target_list l, target* t);
    
/* Main entry point.
 * argc    A count of command-line arguments 
 * argv    The command-line argument valus
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

    size_t      bufsize   = 0;
    char*       line      = NULL;
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

	if (is_target(line) && !in_target) {
	    in_target = 1;

	    t = target_new(target_parsename(line));
	    target_adddep(t, target_parsedep(line));

	} else if (is_target(line) && in_target) {
	    
	    list_append(&l, (cons*) t);
	    t = target_new(target_parsename(line));
	    target_adddep(t, target_parsedep(line));


	} else if (in_target && line[0] == '\t') {
	    
	    target_addrule(t, line);

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
    while (l != NULL) {
	target* tmp = (target*) l;
	l = l->next;
	target_free(tmp, NULL);
    }

    fclose(makefile);
    free(line);

    return EXIT_SUCCESS;
}


/* Process Line
 * argv   The command line to execute
 * 
 * This function calls arg_parse to get the arguments for the given line,
 * then uses fork to execute the command given in the line in the child process.
 * The parent process waits for the child to complete before the function exits.
 */
void processline (char* line) {
 
    int argcp   = -1;
    char** argv = arg_parse(line, &argcp);

    if (argcp  > 0) {
	const pid_t cpid = fork();

	switch(cpid) {

	  case -1: { // error
	      perror("fork");
	      break;
	  }

	  case 0: { // child
	      execvp(argv[0], argv);
	      perror("execvp");
	      exit(EXIT_FAILURE);
	      break;
	  }

	  default: { // parent
	      int   status;
	      const pid_t pid = wait(&status);
	      if(-1 == pid) {
		  perror("wait");
	      } else if (pid != cpid) {
		  fprintf(stderr, "wait: expected process %d, but waited for process %d",
		      cpid, pid);
	      }
	      free(argv);
	      break;
	  }
       } 
    }
}


void processtarget (target* t) {
    str_list sl = target_getrules(t);
    int i = 0;

    while (i < target_getcount(t) && sl != NULL) {
	char* command = str_getdata((str_node*) sl);
	char* line = strndup(command, strlen(command));
	processline(line);
	free(line);
	sl = sl->next;
	i++;
    }
}

void processdep (target_list l, target* t) {
    if (t == NULL)
	return;
    else {
	str_list deps = target_getdepend(t);
	while (deps) {
	    target* t_oth = target_findmatch(l, str_getdata((str_node*) deps));
	    processdep(l, t_oth);

	    deps = deps->next;
	}
	processtarget(t);
    }
}


