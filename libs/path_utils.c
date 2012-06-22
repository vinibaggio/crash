
#include "path_utils.h"

int getpath(const char *src, char *dest, int max_chars) {
	int dest_size = 0;
	int i = 0;
	
	if(!src || !dest) return 0;
	if(max_chars < 0 || max_chars > MAX_STRING_SIZE) {
		#ifdef DEBUG
		printf("WARNING: INVALID max_chars SENT TO readline()!!!\n");
		#endif
		return 0;
	}
	
	strncpy(dest, src, max_chars);
	dest_size = strlen(dest);
	i = dest_size-1;

	while(i >= 0 && dest[i] != '/') {
		dest[i] = 0;
		i--;
	}
	
	return i;
	
}


int getfile(const char *path, char *filename, int max_chars) {
	int i = 0;
	int j = 0;
	
	if(!path || !filename) return 0;
	if(max_chars < 0 || max_chars > MAX_STRING_SIZE) {
		#ifdef DEBUG
		printf("WARNING: INVALID max_chars SENT TO readline()!!!\n");
		#endif
		return 0;
	}
	
	i = strlen(path);

	// Find the slash
	while(i > 0 && path[i] != '/') {
		i--;
	}
	
	// It's not a path
	if(i == 0) return 0;
	
	j = 0;
	i++; // I don't want the slash
	while(path[i] != 0) {
		filename[j] = path[i];
		i++;
		j++;
	}
	
	return j;
	
}


int fileexists(const DIR *dir, const char *filename) {
	int len = strlen(filename);
	struct dirent *dp;
	
	while((dp = readdir((DIR*)dir)) != NULL) {
		if(strlen(dp->d_name) == len && !strcmp(dp->d_name, filename)) {
			return 1;
		}
	}
	
	return 0;
}

/**
 * See if a directory exists
 * /param path The path to check
 */
int direxists(const char *path) {
	DIR *dir = NULL;
	int d = 0;
	dir = opendir(path);
	if(dir) {
		d++;
		closedir(dir);
	}
	return d;
}
