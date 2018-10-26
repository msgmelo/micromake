#ifndef _TARGET_H_
#define _TARGET_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "arg_parse.h"
#include "list.h"

struct target_st;
typedef struct target_st target;
typedef list target_list;

target* target_new(char* name);
void target_free(target* t);
char* target_getname(target* t);
char** target_addrule(target* t, char* rule);

		  


#endif
