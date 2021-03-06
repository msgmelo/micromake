#include <unistd.h>
#include <sys/wait.h>
#include "arg_parse.h"
#include "processline.h"
#include "target.h"
#include "list.h"

/* FUNCTION DESCRIPTIONS IN HEADER FILE */

/* Process Line
 *
 * Attempts to expand the given line, then calls arg_parse before
 * fork and execing to the new process given by line.
 */
void processline (char* line) {

    int argcp   = -1;
    char** argv = NULL;
    int sz      = 1.5*strlen(line);
    char buf[sz];

    if (expand(line, buf, sz) != 0)
	line = buf;

    argv = arg_parse(line, &argcp);

    if (argcp  > 0) {
	const pid_t cpid = fork();

	switch(cpid) {

	  case -1: { // error
	      perror("fork");
	      break;
	  }

	  case 0: { // child
	      argv = arg_IOred(argv, argcp);
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


/* Define Environment
 *
 * This function first finds the left hand of the variable assignment deliminated by '=',
 * then finds the right side of the assignmnet, and calls setenv() on the appropriate char*s. 
 * Returns 0 on a failure and 1 on success.
 */
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
	while (i < len && line[i] != '#') {
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

/* Expand
 *
 * returns 1 on a success, 0 on a failure
 */
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

/* Needs Update
 *
 * Returns true if dependency is newer than target or the target does not exist,
 * otherwise returns false.  
 */
bool needupdate(char* target, char* dep){
    bool ret = true;

    struct stat tinfo;
    struct stat dinfo;

    if (dep == NULL && stat(target, &tinfo) == 0) {
	ret = false;
    } else {
	if (stat(target, &tinfo) == 0 && stat(dep, &dinfo) == 0) {
	    struct timespec tmod = tinfo.st_mtim;
	    struct timespec dmod = dinfo.st_mtim;
	    if (tmod.tv_sec > dmod.tv_sec) { 
		ret = false; 
	    } else if (tmod.tv_sec == dmod.tv_sec && tmod.tv_nsec > dmod.tv_nsec) {
		ret = false;
	    }
	}
    }

    return ret;
}

/* Out of Date
 * 
 * returns true if the target is out of date
 */

bool out_of_date(target* t) {
    bool ret = false;
    str_list deps = target_getdepend(t);
    char* t_name = target_getname(t);

    if (deps == NULL) {
	ret = needupdate(t_name, NULL);
    }


    while (deps) {
	char* d_name = str_getdata((str_node*) deps);	
	ret |= needupdate(t_name, d_name);	
	deps=deps->next;
    }
    return ret;
}

/* Substring
 *
 * allocates space for the returned char*
 */
char* substring(int start, int end, char* str) {
    char sub[end-start];
    int cur = start;
    int i = 0;

    while (cur <= end) {
	sub[i] = str[cur];
	cur++;
	i++;
    }

    sub[i] = '\0';
    return strndup(sub, strlen(sub));
}

/* Process Target
 *
 * Calls processline() on each rule within a target
 */
void processtarget (target* t) {
    if (t == NULL)
	return;
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

/* Process Dependencies
 *
 * Recursively iterates through each target's dependecy in the given 
 * target_list, then processes the target once all the dependencies have
 * been processed.
 */
void processdep (target_list l, target* t) {
    if (t == NULL)
	return;
    else {
	str_list deps = target_getdepend(t);
	while (deps) {
	    char* dep_name = str_getdata((str_node*) deps);
	    target* t_oth = target_findmatch(l, dep_name);
	    processdep(l, t_oth);

	    deps = deps->next;
	    
	}
	if (out_of_date(t))
	    processtarget(t);
    }
}
