#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/times.h>
#include <sys/time.h>
#include "files.h"

#ifndef LOGGING_H
#define LOGGING_H

struct timeval start_time;
FILE *log_fp;

// Returns 0 on success, 1 otherwise.
int init_logging()
{
	// Get log file path from environment variable.
	char *log_file_path = getenv("LOGFILENAME");
	if (log_file_path == NULL)
	{
		return 1; // Failed to read log file path.
	}

	// Open log file.
	if (file_exists(log_file_path) == 0)
	{
		log_fp = fopen(log_file_path, "a"); // Open file in append mode.
	}
	else
		log_fp = fopen(log_file_path, "w"); // Create file.

	// Make sure the file could be opened.
	if (log_fp == NULL)
	{
		return 1;
	}
	// Get program start timestamp.
	gettimeofday(&start_time, NULL);
	return 0;
}

void log_action(char *action, char* description)
{
	// Get current timestamp.
	struct timeval curr_time;
	gettimeofday(&curr_time, NULL);
	// Get relative timestamp to process init.
	suseconds_t timestamp_now = curr_time.tv_usec - start_time.tv_usec;
	// Set file position of the stream to the end of the file.
	fseek(log_fp, 0, SEEK_END);
	double timestamp_now_double = ((double)timestamp_now/1000); // Microseconds -> Miliseconds conversion
	fprintf(log_fp, "%.2f - %8i - %s %s\n", timestamp_now_double, getpid(), action, description);
}

#endif