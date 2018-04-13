#define _GNU_SOURCE // Needed in order to use gnu specific function strcasest
#include <stdio.h>
#include "search_settings.h"
#include "files.h"
#include "search.h"
#include "signals.h"
#include "logging.h"

int main(int argc, char *argv[])
{
	/* First step: read search settings from argv[] */
	Search_Settings *ssettings = get_search_settings(argv);

	/* Is the user input correct? */
	if (!ssettings)
	{ // Print correct program usage.
		printf("Usage: simgrep [options] patterm [file/dir]\n");
		printf("Options (optional): -i -l -n -c -w -r\n");
		return 1;
	}
	/* Did the user enter a path of a file to search at? */
	if (!ssettings->where_to_search)
	{
		// Search pattern at user input.
		char *line;
		size_t buffer_size = 0;
		while (getline(&line, &buffer_size, stdin) != -1)
		{
			if (process_line(line, ssettings) == 0)
				printf("MATCH\n");
			else
				printf("NO MATCH\n");
		}
	}
	/* Setup signal handlers. */
	if (setup_signal_handlers())
	{
		printf("Failed to setup signal handlers. Exiting.\n");
		return 1;
	}

	/* Initialize logging. */
	if (init_logging())
	{
		printf("Failed to initialize logging. Exiting.\n");
		return 1;
	}

	/* Log argument variables. */
	char *arg_variables = "";
	for (char **strPtr = argv; *strPtr != NULL; strPtr++)
	{
		char *str = *strPtr;
		arg_variables = concatenate_strings(arg_variables, str);
		arg_variables = concatenate_strings(arg_variables, " ");
	}
	log_action("COMANDO", arg_variables);

	/* Check if file/dir exists */
	if (file_exists(ssettings->where_to_search) != 0)
	{
		printf("No file or directory could be located at: %s\n", ssettings->where_to_search);
	}

	/* Start Search */
	if (is_regular_file(ssettings->where_to_search))
	{
		process_file(ssettings->where_to_search, ssettings);
	}
	else if (is_directory(ssettings->where_to_search))
	{
		process_directory(ssettings->where_to_search, ssettings);
	}

	/* Wait for all child processes to terminate. */
	wait_for_child_processes();

	return 0;
}