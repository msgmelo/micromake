#include "list.h"
#include <assert.h>
#include <stdio.h>


void list_append( list* l, cons* c) {

    if ((*l) != NULL) {
	while((*l)->next != NULL) {
	    l = &((*l)->next);
	}
	(*l)->next = c;
    } else {
	(*l) = c;
    }
}


static
unsigned list_length_aux(list l, unsigned len) {
    if (l == NULL) 
	return len;
    else
	return list_length_aux(l->next, 1+len);
}

unsigned list_length(list l) {
    return list_length_aux(l, 0);
}


void list_for_each(list l, action_fun action, void* arg) {

    printf("list_for_each\n");
    while (l != NULL) {
	action(l, arg);
	l = l->next;
    }
}


// returns NULL if no match found
cons* list_find(list l, int ismatch(cons* cn, void* ag), void *arg) {
    cons* match = NULL;

    while( !ismatch(l, arg) ) {
	l = l->next;
    }

    match = l;
    return match;
}


void* halloc(size_t sz) {
//    printf("[list] about to malloc\n");
    
    void* p = malloc(sz);

  //  printf("[list] just malloc'd\n");
    if ( p== NULL ) {
	perror("malloc");
	abort();
    }

    //printf("[list] about to return\n");

    return p;
}
