#include "str_node.h"

struct str_node_st {
    str_node* next;
    char* data;
};

str_node* str_new(char* data) {
    str_node* s = halloc(sizeof(str_node));

    s->data = strndup(data, strlen(data));

    return s;
}

void str_print(cons* c, void* arg) {
    str_node* s = (str_node*) c;
    printf("%s\n", s->data);
}

void str_free(str_node* s) {
    free(s->data);
    free(s);
}


char* str_getdata(str_node* s) {
    return s->data;
}




