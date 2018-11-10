#ifndef PROCESSLINE_H_
#define PROCESSLINE_H_

#include "target.h"

/* Process Line
 * line    The command line to execute
 * This function creates a new child process to execute the line 
 * and waits for that process to complete.
 */
void  processline(char* line);

/* Process Target
 * t    the target to process
 * This function calls processline() for each rule in the given target.
 */
void  processtarget(target* t);

/* Process Dependecies
 * l     list of targets
 * t     current target
 * This recursive function calls processtarget() on each of t's dependecies.
 */
void  processdep(target_list l, target* t);

/* Define Environment
 * line   The line in which a environment definition occurs
 * This function determines if there is an environment variable definition in
 * the given line, the calls setenv() if such a definition exists. 
 * Returns 0 on a failure, 1 on success.
 */
int   defenv(char* line);

/* Expand
 * orig    The input string that may contain variables to be expanded
 * new     An output buffer that will contain a copy of orig with all
 *         variables expanded
 * Expands the variables given within the ${} brackets, copies the expanded
 * line into new.
 * Returns 1 on success and 0 upon failure.
 */
int   expand(char* orig, char* new, int newsize);

/* Substring
 * start    The beginning of the desired substring
 * end      The end of the desired substring
 * str      The string in which the substring is contained
 * This functino returns the substring of the given str in the range [start, end]
 */
char* substring (int start, int end, char* str);

#endif
