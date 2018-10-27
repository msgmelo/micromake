#ifndef _TARGET_H_
#define _TARGET_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include "arg_parse.h"
#include "list.h"

#define TARGET_S   1
#define TARGET_T   2
#define TARGET_C   3
#define TARGET_L   4
#define TARGET_END 5


struct                   target_st;
typedef struct target_st target;
typedef list             target_list;

target* target_new(char* name);
void    target_free(target* t, void* arg);
char*   target_getname(target* t);
int     target_getrule_count(target* t);
char**  target_getrules(target* t);

void    target_addrule(target* t, char** rule, int count);
char*   target_parsename(char* line);
bool    target_ismatch(target* t, const char* name);
target* target_findmatch(list l, const char* name);

typedef void (*target_action) (target* t, void* arg);
void tlist_for_each(target_list l, target_action action, void* arg);

int  target_char_class(char c);
bool is_target(char* line);
bool target_q0(char* line);
bool target_q1(char* line);
bool target_q2(char* line);
bool target_q3(char* line);


		  


#endif
