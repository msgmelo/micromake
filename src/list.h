#ifndef LIST_H_
#define LIST_H_

#include <stdlib.h>

typedef
struct cons_st {
    struct cons_st* next;
} cons;

typedef cons* list;

/* List Append
 * l    list to which to append
 * c    struc to append
 *
 * This function appends the given cons cell to the list, creating
 * a new list if the list is empty.
 */
void list_append(list* l, cons* c);

/* List Length
 * l     list whose length to find
 *
 * returns the length of the list
 */
unsigned list_length(list l);

/* List For Each
 * l           list to iterate through
 * action_fun  function to perform on list items
 * arg         arg required by action_fun
 *
 * This function iterates through the list, performing the action_fun
 * function on each cons cell within the list
 */
typedef void (*action_fun) (cons* c, void* args);
void list_for_each(list l, action_fun action, void* arg);

/* List Find
 * l          list to search through
 * ismatch    matching function
 * arg        argument required by matching function
 *
 * This function iterates through the list to find a match.
 * returns the cons cell that matches the given arg.
 */
cons* list_find(list l, int ismatch(cons*, void*), void* arg);

/* Heap Alloc
 * sz    size to allocate to heap
 *
 * Function that allocates to heap and ensures malloc was successful,
 * aborting if not.
 */
void* halloc(size_t sz);


#endif
