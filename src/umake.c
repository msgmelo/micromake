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

/* Define Environment Variable
 *
 */
int defenv(char* line); 

/* Process Dependencies
 * l     The list of targets
 * t     The target whose dependcies will be processed
 * This recursive function  calls processtarget on each of t's dependencies.
 */
void processdep (target_list l, target* t);

/* Expand
 * orig    The input string that may contain variables to be expanded
 * new     An output buffer that will contain a copy of orig with all 
 *         variables expanded
 * newsize The size of the buffer pointed to by new.
 * returns 1 upon success or 0 upon failure. 
 *
 * Example: "Hello, ${PLACE}" will expand to "Hello, World" when the environment
 * variable PLACE="World". 
 */
int expand(char* orig, char* new, int newsize);

/* Substring
 * start    Where the substring starts
 * end      Where the substring ends
 * str      The string whose substring is returned
 * This function returns the substring of the given str in the range [start, end]
 */
char* substring(int start, int end, char* str);


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

	} else if (!is_target(line)) {
	    defenv(line);
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
    char** argv = NULL;
    int sz      = 1.5*strlen(line);
    char buf[sz];

    if (expand(line, buf, sz) == 0)
	argv = arg_parse(line, &argcp);
    else
	argv = arg_parse(buf, &argcp);


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

int defenv(char* line) {
    int ret = 0;
    int i = 0;
    int j = 0;
    int inword = 0;
    int end = 0;
    int len = strlen(line);
    char name[len];
    char value[len];

    while (i < len && !end) {
	if (line[i] == '=')
	    end = 1;
	else {
	    if (!isspace(line[i]) && !inword) {
		inword = 1;
	    }

	    if (!isspace(line[i]) && inword) {
		name[j] = line[i];
		j++;
	    } else if (isspace(line[i]) && inword) {
		inword = 0;
	    }
	}
	i++;
    }

    if (end) { 
	name[j] = '\0';

	j = 0;
	inword = 0;
	while (i < len) {
	    if (!inword && !isspace(line[i])) {
		inword = 1;
	    }

	    if (inword) {
		value[j] = line[i];
		j++;
	    }
	    i++;
	}

	value[j] = '\0';

	ret = setenv(name, value, 1);
    }

    return ret;
}

//int defenv(char* line) {
//
//    char* left = strtok(line, "=");
//    char* right = strtok(NULL, "=");
//
//    if (right == NULL) {
//	return 0;
//    } 
//
//    // remove trailing spaces
//    int inword = 0;
//    int end = 0;
//    int i = 0;
//    while (i < strlen(left) && !end) {
//	if (!isspace(left[i]) && !inword) {
//	    inword = 1;
//	} else if (isspace(left[i]) && inword) {
//	    end = 1;
//	} 
//	i++;
//    }
//    left[i] = '\0';
//
//    // remove heading spaces
//    i = 0;
//    int j = 0;
//    inword = 0;
//    char assignment[strlen(left)];
//    while (i < strlen(line)) {
//	if (!isspace(line[i]) && !inword) {
//	    inword = true;
//	}
//
//	if (inword) {
//	    assignment[j] = line[i];
//	    j++;
//	}
//	i++;
//    }
//
//    int ret = setenv(left, assingment, 1)
//
//    
//    return ret;
//}
//

int expand(char* orig, char* new, int newsize) {

    bool inexpand = false;
    char* expansion = NULL;
    int loc[2];
    int i = 0;
    int j = 0;


    while(orig[i] && j < newsize) {
	if (orig[i] == '$' && !inexpand) {
	    if (orig [i+1] && orig[i+1] == '{') {
		loc[0] = i + 2;
		inexpand = true;
	    }
	} else if (orig[i]  == '}' && inexpand) {
	    loc[1] = i - 1;
	    char* word = substring(loc[0], loc[1], orig);
	    expansion = getenv(word);
	    free(word);
	    if (expansion != NULL) {
		int k;
		for (k = 0; k < strlen(expansion); k++) {
		    new[j] = expansion[k];
		    j++;
		}
	    }
	    inexpand = false;
	} else if (!inexpand) {
	    new[j] = orig[i];
	    j++;
	} 
	i++;
    }
    new[j] = '\0';

    if (inexpand) {
	printf("Bracket mismatch in %s\n", orig);
	return 0;
    } else {
	return 1;
    }
}


char* substring(int start, int end, char* str) {
    char sub[end-start];
    int cur = start;
    int i = 0;

    while (cur <= end) {
	sub[i] = str[cur];
	i++;
	cur++;
    }

    sub[i] = '\0';
    return strndup(sub, strlen(sub));
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


