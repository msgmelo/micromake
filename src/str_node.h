#ifndef _STR_NODE_H_
#define _STR_NODE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include "list.h"


struct                    str_node_st;
typedef struct str_node_st str_node;
typedef list               str_list;

str_node* str_new(char* data);

void      str_print(cons* c, void* arg);

void      str_free(str_node* s);

char*     str_getdata(str_node* s);




#endif
