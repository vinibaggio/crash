#ifndef _STR_UTILS_H_
#define _STR_UTILS_H_

#include <stdio.h>
#include <string.h>

#define MAX_STRING_SIZE 1000

/**
 * Clean spaces and newlines at the beginning and the end. Note that
 * this is UNIX ONLY. Also works on MacOSX 10.0 and above.
 * This won't work with unicode characters.
 *
 * /param buffer the string you want to be cleaned
 * /param max_chars the max number of possible characters
 * /return the final size of the string
 */
int cleanstring(char *buffer, int max_chars);
	
/**
 * Read a whole line from the input
 * /param buffer a buffer that will receive the line
 * /param max_chars the number of characters that will be read
 * /return the number of read characters, counting the NULL character on the end
 */
int readline(char *buffer, int max_chars);


#endif /* _STR_UTILS_H_ */
