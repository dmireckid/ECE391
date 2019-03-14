#ifndef _TERM_DRIVER_H
#define _TERM_DRIVER_H

#include "lib.h"

#define LINE_BUFFER_SIZE	128

int buffer_count;
char line_buffer[LINE_BUFFER_SIZE];

/* Add a character to the line buffer */
void type_to_buffer(char input);

/* Remove a character from the line buffer */
void remove_from_buffer();

/* Reads the buffer and calls a command based off of it */
void buffer_command();

/* Clear the line buffer*/
void clear_buffer();

#endif
