
#include "str_utils.h"

/**
 * Clean spaces and newlines at the beginning and the end. Note that
 * this is UNIX ONLY. Also works on MacOSX 10.0 and above.
 * This won't work with unicode characters.
 *
 * /param buffer the string you want to be cleaned
 * /param max_chars the max number of possible characters
 * /return the final size of the string
 */
int cleanstring(char *buffer, int max_chars) {
	int linesize = 0;
	int i = 0, j = 0;
	
	if(!buffer) {
		return 0;
	}
	
	if(max_chars < 0 || max_chars > MAX_STRING_SIZE) {
		#ifdef DEBUG
		printf("WARNING: INVALID max_chars SENT TO readline()!!!\n");
		#endif
		return 0;
	}
	linesize = strlen(buffer);
	
	// Look for interesting characters
	i = 0;
	while(buffer[i] == ' ' || buffer[i] == 0x0A) {
		i++;
		linesize--;
	}
	// Move everything to the beginning
	j = 0;
	// If we need to move at all
	if(i != 0) {
		while(j < linesize) {
			buffer[j] = buffer[i];
			buffer[i] = 0x00;
			j++; i++;
		}
	}
	
	// Now clean the end
	i = linesize-1;
	while(buffer[i] == ' ' || buffer[i] == 0x0A) {
		buffer[i] = 0x00;
		i--;
		linesize--;
	}
	
	return linesize;
}


/**
 * Read a whole line from the input
 * /param buffer a buffer that will receive the line
 * /param max_chars the number of characters that will be read
 * /return the number of read characters, counting the NULL character on the end
 */
int readline(char *buffer, int max_chars) {
	if(!buffer) return 0;
	if(max_chars < 0 || max_chars > MAX_STRING_SIZE) {
		#ifdef DEBUG
		printf("WARNING: INVALID max_chars SENT TO readline()!!!\n");
		#endif
		return 0;
	}
	fgets(buffer, max_chars, stdin);
	return cleanstring(buffer, max_chars);

}
