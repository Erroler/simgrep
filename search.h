#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include "search_settings.h"
#include "utilities.h"
#include "signals.h"

#ifndef SEARCH_H
#define SEARCH_H


/** Returns 0 if there was a match or 1 if not.*/
int process_line(char *line, const Search_Settings *ssettings)
{
	char *pos = line;
	// Search string for occurrences of pattern.
	while ((pos = (ssettings->case_insensitive ? strcasestr(pos, ssettings->pattern) : strstr(pos, ssettings->pattern))) != NULL) // Match!!!!
	{
		if (ssettings->match_must_be_word) // Check if the match is a word by the grep definition of it.
		{
			if (pos == line || pos == line - strlen(ssettings->pattern))
			{
				return 0;
			}
			else if ( !is_word_constituent(*(pos-1)) && !is_word_constituent(*(pos + strlen(ssettings->pattern))) )
			{
				return 0;
			}
		}
		else 
		{
			return 0;
		}
		pos++;
	}
	
	return 1;
}

void process_file(char *path, const Search_Settings *ssettings)
{
	log_action("ABERTO FICHEIRO", path);
	FILE *fp;
	/* Open the file in order to read it */
	fp = fopen(path, "r");
	/* Read the file */
	ssize_t chars_read;
	size_t buffer_size = 0;
	char *line = NULL; // getline will allocate the needed memory.
	unsigned int line_number = 0;
	int total_number_of_matches = 0;
	while ((chars_read = getline(&line, &buffer_size, fp)) != -1)
	{
		if (process_line(line, ssettings) == 0) // Hooray! Match
		{
			if (ssettings->only_show_file_names)
			{
				printf("%s\n", path);
				free(line);
				return;
			}
			else if (ssettings->show_lines_number_matched)
			{
				printf("%s:%u:%s", path, line_number, line);
			}
			else if(!ssettings->show_number_of_lines_matched)	printf("%s", line);
		}
		line_number++;
		total_number_of_matches++;
	}

	if (ssettings->show_number_of_lines_matched)
	{
		printf("%s: %d\n", path, total_number_of_matches);
	}
	log_action("ABERTO FICHEIRO", path);
}

void process_directory(char *path, const Search_Settings *ssettings)
{
	log_action("DIRETORIO ABERTO", path);
	// Declare needed structures for directory parsing.
	DIR *dir;
	struct dirent *dentry;
	struct stat stat_entry;
	// Open directory
	if((dir = opendir(path)) == NULL) return;

	// Iterate files in directory
	while( (dentry = readdir(dir)) != NULL)
	{
		char *file_name = dentry->d_name;
		// Get "full" relative path of the file
		char *absolute_path = concatenate_strings("", path);
		absolute_path = concatenate_strings(absolute_path, "/");
		absolute_path = concatenate_strings(absolute_path, file_name);
		// Get file info
		stat(absolute_path, &stat_entry);
		if(S_ISREG(stat_entry.st_mode)) // Found a regular file!
		{
			// Process file
			process_file(absolute_path, ssettings);
		}
		if(S_ISDIR(stat_entry.st_mode) && (ssettings->recursive)) // Found a Directory!
		{
			// Make sure directory is a subdirectory.
			if(file_name[0] == '.') continue;
			// Process file
			int pid = fork(); 	// Analyse directory in a recursive way.
			if(pid == 0) 		// Create a child process and make him search the directory
			{
				setpgid( (pid_t) 0, get_child_pgid() );	// Change the process group of the child.
				reset_number_of_childs();
				process_directory(absolute_path, ssettings);
				return;
			}
			else	// Code that will be executed by the father.
			{
				new_child_was_created();
			}
		}
		free(absolute_path);
	}
	closedir(dir);
	log_action("DIRETORIO FECHADO", path);
}

#endif