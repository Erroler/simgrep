#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef FILES_H
#define FILES_H

/** Checks if a file exists given its path. Returns 0 if it does exist and a different value otherwise. */
int file_exists(const char *path){
	struct stat file_stat;
	return stat(path, &file_stat);
}

/** Returns 0 if the file is a regular file (not a directory or a fifo) or a different value otherwise. */
int is_regular_file(const char* path)
{
	struct stat file_stat;
	stat(path, &file_stat);
	return S_ISREG(file_stat.st_mode);
}

/** Returns 0 if the file is a directory or a different value otherwise. */
int is_directory(const char* path)
{
	struct stat file_stat;
	stat(path, &file_stat);
	return S_ISDIR(file_stat.st_mode);
}

#endif