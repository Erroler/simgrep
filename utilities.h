#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef UTILITIES_H
#define UTILITIES_H

// Returns 1 if a character is word consistuent, 0 if not.
int is_word_constituent(char c)
{
	if(c >= 'a' && c <= 'z')	return 1;
	if(c >= 'A' && c <= 'Z')	return 1;
	if(c >= '0' && c <= '9')	return 1;
	if(c == '_')				return 1;
	return 0;
}

char* concatenate_strings(const char* s1, const char* s2)
{
    // Get the length of the strings.
    size_t s1_size = strlen(s1), s2_size = strlen(s2);

    // Allocate memory for the new string..
    char *new_string = malloc(s1_size + s2_size);

    // Copy "child" strings into new string.
    memcpy(new_string, s1, s1_size);
    memcpy(new_string + s1_size, s2, s2_size);
    
    // Return a pointer to the new string.
    return new_string;
}


#endif