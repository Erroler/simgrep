#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#ifndef SEARCH_SETTINGS_H
#define SEARCH_SETTINGS_H

typedef struct search_settings
{
	char *pattern;
	char *where_to_search;
	int case_insensitive;
	int only_show_file_names;
	int show_lines_number_matched;
	int show_number_of_lines_matched;
	int match_must_be_word;
	int recursive;
} Search_Settings;

Search_Settings *get_search_settings(char *argv[])
{
	/* Allocate memory using calloc since it will initialize the memory region to 0.
     * That is helpful because it will set all the boolean ints in our struct to false (0). */
	Search_Settings *ssettings = calloc(1, sizeof(Search_Settings));

	// Iterate process arguments.
	for (char **strPtr = &argv[1]; *strPtr != NULL; strPtr++) // No need to iterate over argv[0] since that holds the programs name!
	{
		char *str = *strPtr;
		if (str[0] == '-') // Got a option parameter!
		{
			switch (str[1])
			{
			case 'i':
				ssettings->case_insensitive = 1;
				break;
			case 'l':
				ssettings->only_show_file_names = 1;
				break;
			case 'n':
				ssettings->show_lines_number_matched = 1;
				break;
			case 'c':
				ssettings->show_number_of_lines_matched = 1;
				break;
			case 'w':
				ssettings->match_must_be_word = 1;
				break;
			case 'r':
				ssettings->recursive = 1;
				break;
			}
		}
		else // Process pattern or file/dir.
		{
			if (ssettings->pattern) // If we have already processed the pattern that means we have found the file/dir name.
			{
				ssettings->where_to_search = str;
			}
			else // If not, that means str holds the pattern.
			{
				ssettings->pattern = str;
			}
		}
	}
	// If we have not found both the pattern that means the user input wasn't correct.
	if (!ssettings->pattern)
	{
		free(ssettings);
		return NULL;
	}
	return ssettings; // All Good!
}

#endif