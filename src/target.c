#include "target.h"

/* FUNCTION DESCRIPTIONS IN HEADER FILE */


struct target_st {
    target* next;
    char* name;
    str_list rules;
    str_list depends;
    int rule_count;
};


target* target_new(char* name) {
    target* t = malloc(sizeof(target));

    if (t != NULL) {
	t->next = NULL;
	char* t_name = strndup(name, strlen(name));
	t->name = t_name;
	t->rules = NULL;
	t->depends = NULL;
	t->rule_count = 0;
    }
    
    return t;
}

void target_print(cons* c, void* arg) {
    target* t = (target*) c;
    printf("%s\n", t->name);
}


static void rules_free(str_list l, int count) {

    int i = 0;
    str_node* tmp = NULL;

    while (l != NULL && i < count) {
	tmp = (str_node*) l;
	l = l->next;
	free(str_getdata(tmp));
	free(tmp);
	i++;
    }
}


void target_free(target* t, void* arg) {
    rules_free(t->rules, t->rule_count);
    free(t->name);    
    free(t);
}


char* target_getname(target* t) {
    return t->name;
}

int target_getcount(target* t) {
    return t->rule_count;
}

str_list target_getrules(target* t) {
    return t->rules;
}

str_list target_getdepend(target* t) {
    return t->depends;
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
//static int copy_seek(char** dest, char** source, int dest_offset, int max) {
//    int i = 0;
//
//    for ( ; dest_offset < max; dest_offset++) {
//	dest[dest_offset] = halloc(sizeof(char) * strlen(source[i]));
//	strcpy(dest[dest_offset], source[i]);
//	i++;
//    }
//
//    return dest_offset;
//}
//
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

//void target_addrule(target* t, char** rule, int count) {
//    assert(t != NULL && "cannot add rule to NULL target");
//    
//    char** new_rules = NULL;
//    
//    if (t->rules == NULL) {
//	
//	new_rules = halloc((count + 1)*sizeof(char*));
//	copy_seek(new_rules, rule, 0, count);
//    } else {
//	char** old_rules = t->rules;
//	int new_count = count + t->rule_count;
//	new_rules = halloc((new_count + 1)*sizeof(char*));
//	
//	int offset = copy_seek(new_rules, old_rules, 0, t->rule_count);
//	copy_seek(new_rules, rule, offset, new_count);
//
//	new_rules[new_count] = NULL;
//
//	rules_free(old_rules, t->rule_count);
//
//	t->rule_count = new_count;
//    }
//    
//    t->rules = new_rules;
//    t->rule_count++;
//   
//}
//
//rule* target_makerule(char** command, int count) {
//
//    rule* r = halloc(sizeof(rule));
//
//    r->next = NULL;
//    r->command = command;
//    r->count = count;
//
//    return r;
//}
//
//void target_addrule(target* t, char** rule, int count) {
//    assert(t != NULL && "cannot add rule to NULL target");
//
//    rule* r_list = NULL;
//    r_list = t->rules;
//
//    rule* new_rule = target_makerule(rule, count);
//    list_append(&r_list, new_rule);
//    t->rule_count++;
//
//
//}
//
//void target_addrule(target* t, char** command, int count) {
//    assert(t != NULL && "cannot add rule to NULL target");
//
//    rule_list r_list = t->rules;
//
//    rule* new_rule = target_makerule(command, count);
//    list_append(&r_list, (cons*) new_rule);
//    t->rule_count++;
//}
//

void target_addrule(target* t, char* line) {
    assert(t != NULL && "cannot add rule to NULL target");

    str_list l = t->rules;
    str_node* rule = str_new(line);
    list_append(&l, (cons*) rule);
    t->rules = l;
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
    char name[strlen(line)];
    int in_word = 0;
    int fin = 0;

    while ((line[i] != ':') && !fin) {

       	if (!isspace(line[i]))	{
	    in_word = 1;
	    name[j] = line[i];
	    j++;
	} else if (isspace(line[i]) && in_word) {
	    fin = 1;
	}
	i++;
    }


    name[j] = '\0';
    char* name_final = malloc(sizeof(char) * (j + 1));
    
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
	} 
	l = l->next;
    }

    if (found)
	return t;
    else
	return NULL;
}
   
