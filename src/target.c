#include "list.h"
#include "target.h"


struct target_st {
    target* next;
    char* name;
    char** rules;
    int rule_count;
};


target* target_new(char* name) {
    target* t = malloc(sizeof(target));

    if (t != NULL) {
	t->name = malloc(strlen(name));
	if (t->name == NULL) {
	    perror("malloc");
	    exit(1);
	}
	strncpy(t->name, name, strlen(name));
	t->rules = NULL;
	t->rule_count = 0;
    }
    
    return t;
}

static void rules_free(char** rules, int count) {

    int i;
    for (i = 0; i < count; i++) {
	free(rules[i]);
    }

    free(rules);
}


void target_free(target* t) {
    
    free(t);
}



char* target_getname(target* t) {
    return t->name;
}

static int copy_seek(char** dest, char** source, int dest_offset, int max) {
    int i = 0;

    for ( ; dest_offset < max; dest_offset++) {
	dest[dest_offset] = malloc(strlen(source[i] + 1));
	if (dest[dest_offset] == NULL) {
	    perror("malloc");
	    exit(1);
	}
	strncpy(dest[dest_offset], source[i], strlen(dest[dest_offset]));
    }

    return dest_offset;
}

char** target_addrule(target* t, char* rule) {
    char** new_rules = NULL;

    if (t->rules == NULL) {
	new_rules = arg_parse(rule, &(t->rule_count));
    } else {
	char** old_rules = t->rules;
	int new_count = 0;
	char** copy_rules = arg_parse(rule, &new_count);
	new_count = new_count + t->rule_count;
	new_rules = malloc((new_count + 1)*sizeof(char*));
	
	int offset = copy_seek(new_rules, old_rules, 0, t->rule_count);
	copy_seek(new_rules, copy_rules, offset, new_count);

	new_rules[new_count] = NULL;

	rules_free(old_rules, t->rule_count);

	t->rule_count = new_count;
    }
    return new_rules;
}



   
