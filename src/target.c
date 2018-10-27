#include "list.h"
#include "target.h"
#include "arg_parse.h"

/* FUNCTION DESCRIPTIONS IN HEADER FILE */

struct target_st {
    target* next;
    char* name;
    char** rules;
    int rule_count;
};


target* target_new(char* name) {
    target* t = malloc(sizeof(target));

    if (t != NULL) {
	t->name = strndup(name, strlen(name));
	t->rules = NULL;
	t->rule_count = 0;
    }
    
    return t;
}

void target_print(cons* c, void* arg) {
    target* t = (target*) c;
    printf("%s\n", t->name);
}


static void rules_free(char** rules, int count) {

    int i;
    for (i = 0; i < count; i++) {
	free(rules[i]);
    }

    free(rules);
}


void target_free(target* t, void* arg) {
    rules_free(t->rules, t->rule_count);
    free(t->name);    
    free(t);
}


char* target_getname(target* t) {
    return t->name;
}

int target_getrule_count(target* t) {
    return t->rule_count;
}

char** target_getrules(target* t) {
    return t->rules;
}


/* Copy Seek
 * dest          where char** data will be copied
 * source        where char** data will be copied from
 * dest_offset   where to start in destination
 * max           where to end in destination
 *
 * This is a helper function for adding a rule to a given target. It allocates
 * space to each char* string pointed to, and copies the source char* pointer into
 * the new array. 
 * returns where in the destination the copy finished
 */
static int copy_seek(char** dest, char** source, int dest_offset, int max) {
    int i = 0;

    for ( ; dest_offset < max; dest_offset++) {
	dest[dest_offset] = halloc(sizeof(char) * strlen(source[i]));
	strcpy(dest[dest_offset], source[i]);
	i++;
    }

    return dest_offset;
}

/* Target Add Rule
 * t             Pointer to target to add rule to
 * rule          Rule to add
 * count         Number of arguments in given rule
 *
 * This function adds the given rule to the target, allocating and freeing
 * the rule space as necessary. If the target has no given rules, it simply 
 * calls copy_seek() described above. Otherwise, it allocates space for the
 * old rules and new rules, then copies the old rules into the new rule array.
 * It then copies the new rules into the new array, before freeing the original 
 * rules. The rule_count is updated.
 */
void target_addrule(target* t, char** rule, int count) {
    assert(t != NULL && "cannot add rule to NULL target");
    
    char** new_rules = NULL;
    
    if (t->rules == NULL) {
	
	new_rules = halloc((count + 1)*sizeof(char*));
	copy_seek(new_rules, rule, 0, count);
    } else {
	char** old_rules = t->rules;
	int new_count = count + t->rule_count;
	new_rules = halloc((new_count + 1)*sizeof(char*));
	
	int offset = copy_seek(new_rules, old_rules, 0, t->rule_count);
	copy_seek(new_rules, rule, offset, new_count);

	new_rules[new_count] = NULL;

	rules_free(old_rules, t->rule_count);

	t->rule_count = new_count;
    }
    
    t->rules = new_rules;
    t->rule_count++;
   
}

int target_char_class(char c){
    if (c == '\t')
	return TARGET_T;
    else if (isspace(c))
	return TARGET_S;
    else if(c == ':')
	return TARGET_C;
    else if(c == '\0')
	return TARGET_END;
    else
	return TARGET_L;
}


bool is_target(char* line) {
    return target_q0(line);
}

bool target_q0(char* line) {
    switch(target_char_class(*line)) {
	case TARGET_S: 
	    return target_q0(&line[1]);
	case TARGET_T:
	    return false;
	case TARGET_C:
	    return false;
	case TARGET_L:
	    return target_q1(&line[1]);
	case TARGET_END:
	    return false;
    }
    return false;
}

bool target_q1(char* line) {
    switch(target_char_class(*line)) {
	case TARGET_S: 
	    return target_q2(&line[1]);
	case TARGET_T:
	    return false;
	case TARGET_C:
	    return target_q3(&line[1]);
	case TARGET_L:
	    return target_q1(&line[1]);
	case TARGET_END:
	    return false;
    }
    return false;
}

bool target_q2(char* line) {
    switch(target_char_class(*line)) {
	case TARGET_S: 
	    return target_q2(&line[1]);
	case TARGET_T:
	    return false;
	case TARGET_C:
	    return target_q3(&line[1]);
	case TARGET_L:
	    return false;
	case TARGET_END:
	    return false;
    }
    return false;
}

bool target_q3(char* line) {
    return true;
}


char* target_parsename(char* line) {
    int i = 0;
    int j = 0;
    char name[] = "";
    int in_word = 0;

    while (line[i] != ':') {

       	if (!isspace(line[i]))	{
	    in_word = 1;
	    name[j] = line[i];
	    j++;
	} else if (isspace(line[i]) && in_word) {
	    break;
	}
	i++;
    }

    name[j] = '\0';
    char* name_final = malloc(sizeof(char) * j);
    strncpy(name_final, name, j);

    return name_final;
}


bool target_ismatch(target* t, const char* name) {
    if (strcmp(name, t->name) == 0)
	return true;
    else
	return false;
}

target* target_findmatch(list l, const char* name) {
    target* t = NULL;
    bool found = false;

    while(!found && l != NULL) {
	t = (target*) l;
	if (target_ismatch(t, name)) {
	    found = true;
	    break;
	} 
	l = l->next;
    }

    if (found)
	return t;
    else
	return NULL;
}
   
