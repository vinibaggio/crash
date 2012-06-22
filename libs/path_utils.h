#ifndef _PATH_UTILS_H_
#define _PATH_UTILS_H_

#include <string.h>
#include <dirent.h>

#include "str_utils.h"


/**
 * Get only the path from the string, like if we pass
 * /bin/ls
 * We get /bin/
 * /param src The source path
 * /param dest The destination string
 * /param max_chars Maximum number of characters
 * /return the number of characters, counting the NULL character on the end
 */
int getpath(const char *src, char *dest, int max_chars);
/**
 * Get only the file from the string, like if we pass
 * /bin/ls
 * We get ls
 * /param path The source path
 * /param filename Where the file will be stored
 * /param max_chars Maximum number of characters
 * /return the number of characters, counting the NULL character on the end
 */
int getfile(const char *path, char *filename, int max_chars);

/**
 * See if the file exists
 * /param dir The directory to be looked into
 * /param filename The file to be looked for
 * /return True if exists, else False
 */
int fileexists(const DIR *dir, const char *filename);

/**
 * See if a directory exists
 * /param path The path to check
 */
int direxists(const char *path);

#endif /* _PATH_UTILS_H_ */
