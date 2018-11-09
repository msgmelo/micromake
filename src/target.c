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


void target_addrule(target* t, char* line) {
    assert(t != NULL && "cannot add rule to NULL target");

    str_list l = t->rules;
    str_node* rule = str_new(line);
    list_append(&l, (cons*) rule);
    t->rules = l;
    t->rule_count++;

}

void target_adddep(target* t, str_list dep) {
    assert(t != NULL && "Cannot add dependency to NULL target");

    t->depends = dep;
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

str_list target_parsedep(char* line) {
    str_list  l      = NULL;
    str_node* dep    = NULL;
    char      word[strlen(line)];
    bool      indep  = false;
    bool      inword = false;
    int       i      = 0;
    int       j      = 0;

    while (!indep) {
        if (line[i] == ':')
            indep = true;
	i++;
    }

    while (line[i] != '\0') {
	if (isspace(line[i]) && inword) {
	    word[j] = '\0';
	    dep = str_new(word);
	    list_append(&l, (cons*) dep);
	    j = 0;
	    inword = false;
	} else if (!isspace(line[i]) && !inword) {
	    inword = true;
	}

	if (!isspace(line[i]) && inword) {
	    word[j] = line[i];
	    j++;
	}

	i++;
    }



    return l;
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
   
