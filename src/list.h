#ifndef LIST_H_
#define LIST_H_

typedef
struct cons_st {
    struct cons_st* next;
} cons;

typedef cons* list;

void list_append(list* l, cons* c);
unsigned list_length(list l);

typedef void (*action_fun) (cons* c, void* args);
void list_for_each(list l, action_fun action, void* arg);

cons* list_find(list l, int ismatch(cons*, void*), void* arg);


#endif
