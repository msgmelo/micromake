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

/* String New
 * data    data to be contained within the str_node
 * returns a pointer to a new str_node with the given data
 */
str_node* str_new(char* data);

/* String Print
 * c     The str_node to print
 *
 * prints the given str_node
 */
void      str_print(cons* c);

/* String free
 * s    str_node to free
 * Frees the str_node and the data it contains
 */
void      str_free(str_node* s);

/* String List Free
 * sl    The str_list to be freed
 * Frees each str_node within the str_list
 */
void      strlist_free(str_list sl);

/* String Get Data
 * s    The string whose data to retrive
 * Returns the data of the given str_node
 */
char*     str_getdata(str_node* s);




#endif
