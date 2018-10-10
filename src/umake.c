/* CSCI 347 micro-make
 * 
 * 09 AUG 2017, Aran Clauson
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>

/* CONSTANTS */


/* PROTOTYPES */

/* Process Line
 * line   The command line to execute.
 * This function interprets line as a command line.  It creates a new child
 * process to execute the line and waits for that process to complete. 
 */
void processline(char* line);

/* Argument Parse
 * This function returns an array of new pointers that point to the beginning
 * character of each word in a line, ignoring whitespace.
 */
char** arg_parse(char* line);

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

  FILE* makefile = fopen("./uMakefile", "r");

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

  free(line);
  return EXIT_SUCCESS;
}


/* Process Line
 * 
 */
void processline (char* line) {
  
  const pid_t cpid = fork();
  switch(cpid) {

  case -1: {
    perror("fork");
    break;
  }

  case 0: {
    execlp(line, line, (char*)(0));
    perror("execlp");
    exit(EXIT_FAILURE);
    break;
  }

  default: {
    int   status;
    const pid_t pid = wait(&status);
    if(-1 == pid) {
      perror("wait");
    }
    else if (pid != cpid) {
      fprintf(stderr, "wait: expected process %d, but waited for process %d",
              cpid, pid);
    }
    break;
  }
  }
}


char** arg_parse(char* line)
{
    int count = 0;
    int i = 0;
    int word = 0; // stores whehter iterator is currently within a word
    int *tmp = NULL; // stores the locations beginnings of the words
    
    // count number of words in line
    while(line[i] != '\0') {
        
        if (isspace(line[i])) {
            word = 0;
        } else if (!word) {
            word = 1;
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

    // copy tmp array into malloc'd args
    int loc = 0;

    for (i = 0; i < count; i++) {
        loc = tmp[i];
        args[i] = &line[loc];
    }

    return args;
}
