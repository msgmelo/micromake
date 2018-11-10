#include <unistd.h>
#include <sys/wait.h>
#include "arg_parse.h"
#include "processline.h"
#include "target.h"
#include "list.h"


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

