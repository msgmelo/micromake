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


/* CONSTANTS */

/* PROTOTYPES */

/* Process Line
 * line   The command line to execute.
 * This function interprets line as a command line.  It creates a new child
 * process to execute the line and waits for that process to complete. 
 */
void processline(char* line);

/* Main entry point.
 * argc    A count of command-line arguments 
 * argv    The command-line argument valus
 *
 * Micro-make (umake) reads from the uMakefile in the current working
 * directory.  The file is read one line at a time.  Lines with a leading tab
 * character ('\t') are interpreted as a command and passed to processline minus
 * the leading tab.
 */
int main(int argc, const char* argv[]) {

    FILE* makefile  = fopen("./uMakefile", "r");

    size_t  bufsize = 0;
    char*   line    = NULL;
    ssize_t linelen = getline(&line, &bufsize, makefile);

    while(-1 != linelen) {

        if(line[linelen-1]=='\n') {
            linelen -= 1;
            line[linelen] = '\0';
        }

	if(line[0] == '\t') 
            processline(&line[1]);

        linelen = getline(&line, &bufsize, makefile);

    }

    fclose(makefile);
    free(line);

    return EXIT_SUCCESS;
}


/* Process Line
 * line   The command line to execute
 * 
 * This function calls arg_parse to get the arguments for the given line, 
 * then uses fork to execute the command given in the line in the child process.
 * The parent process waits for the child to complete before the function exits.
 */
void processline (char* line) {
 
    int argcp       = -1;
    char** argv     = arg_parse(line, &argcp);
    
    if (argcp  > 1) {
	const pid_t cpid = fork();

	switch(cpid) {

	  case -1: { // error
	      perror("fork");
	      free(argv);
	      break;
	  }

	  case 0: { // child
	      execvp(argv[0], argv);
	      perror("execvp");
	      free(argv);
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
