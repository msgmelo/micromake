#ifndef PROCESSLINE_H_
#define PROCESSLINE_H_

#include "target.h"


void  processline(char* line);

void  processtarget(target* t);

void  processdep(target_list l, target* t);

int   defenv(char* line);

int   expand(char* orig, char* new, int newsize);

char* substring (int start, int end, char* str);

#endif
