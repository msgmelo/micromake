#ifndef _TARGET_H_
#define _TARGET_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include "arg_parse.h"
#include "str_node.h"
#include "list.h"

#define TARGET_S   1
#define TARGET_T   2
#define TARGET_C   3
#define TARGET_L   4
#define TARGET_END 5


struct                   target_st;
typedef struct target_st target;
typedef list             target_list;


/* New Target
 * name    Name to give new target
 *
 * returns a pointer to a new target with the given name.
 */
target* target_new(char* name);

/* Print Target
 * c       The target to print
 * arg     NULL
 *
 * Prints the name of the given target
 */
void    target_print(cons* c, void* arg);

/* Target Free
 * t        Target to free
 *
 * Frees the target and the fields within the target struct
 */
void    target_free(target* t);

/* Target Get X
 * t        The target whose field to retrieve
 *
 * This block of functions returns the appropriate field of the target
 * given by the pointer.
 */ 
char*    target_getname(target* t);
int      target_getcount(target* t);
str_list target_getrules(target* t);
str_list target_getdepend(target* t);

/* Target Add Rule
 * t        Pointer to target to add rule to
 * rule     Rule to add
 * count    Number of arguments in the given rule
 *
 * This functions adds the given rule to the target, allocating and freeing
 * rule space as necessary.
 */
void    target_addrule(target* t, char* rule);
void    target_adddep(target* t, str_list dep);

/* Target Parse Name
 * line     Where to parse name from
 *
 * This function returns the target name found in the given line
 */
char*    target_parsename(char* line);

str_list target_parsedep(char* line);

/* Target Is Match
 * t        Target to compare
 * name     Name to compare target to
 *
 * This function compares the name of the given target to the desired name,
 * and returns true if it is a match.
 */
bool    target_ismatch(target* t, const char* name);

/* Target Match Find
 * l       List of targets to search through
 * name    Name to be searched for
 */
target* target_findmatch(list l, const char* name);

/* Target Classify Character
 * c      Character to classify
 *
 * This function classifies the given char, and returns the int value given to each 
 * type of classification, listed below:
 * TARGET_S   : character is a space, but not a tab character
 * TARGET_T   : character is a tab
 * TARGET_C   : character is a colon
 * TARGET_L   : any character not a space or colon or null
 * TARGET_END : character is a null terminator
 */
int  target_char_class(char c);

/* Is Target
 * line    Line to determine if is a target
 *
 * Returns true if given line is a target
 */
bool is_target(char* line);


/* Target State Q*
 * line     Line to determine if is a target
 *
 * This series of functions serve to describe the states of the in-class 
 * created state machine. The states are described as follows:
 * Q0 : ignores spaces (but not tabs). Advances to Q1 if non-space non-tab 
 *      character is found
 * Q1 : ignores non-space non-tab characters. Advances to Q2 if space 
 *      character is found. Advances to Q3 if colon character is found
 * Q2 : ignores spaces (but not tabs). Advances to Q3 if colon character is 
 *      found
 * Q3 : returns true regardless of character classification
 */
bool target_q0(char* line);
bool target_q1(char* line);
bool target_q2(char* line);
bool target_q3(char* line);


		  


#endif
