#include "term_driver.h"
#include "keyboard.h"

int buffer_count = 0;
uint8_t buffer_output_enabled = 0;

/* 
 * type_to_buffer(char input)
 *   Description: Adds a character into the line buffer
 *         Input: input - character to be added into the line buffer
 *        Output: None
 *        Return: None
 *  Side Effects: Modifies the line buffer and increments buffer_count
 */
void type_to_buffer(char input) {
	/* if the pointer to the line buffer is NULL, return */
	if (line_buffer == NULL)
		return;
	/* if the line buffer has one slot left and the input isn't Enter, don't do anything */
	if (buffer_count == LINE_BUFFER_SIZE-1 && input != '\n')
		return;
	/* if the line buffer is full, don't do anything */
	if (buffer_count == LINE_BUFFER_SIZE)
		return;
	
	*(line_buffer+buffer_count) = input;
	buffer_count++;
}

/* 
 * remove_from_buffer()
 *   Description: Removes a character into the line buffer
 *         Input: None
 *        Output: None
 *        Return: None
 *  Side Effects: Modifies the line buffer and decrements buffer_count
 */
void remove_from_buffer() {
	/* if the pointer to the line buffer is NULL, return */
	if (line_buffer == NULL)
		return;
	/* if the line buffer is empty, don't do anything */
	if (buffer_count == 0)
		return;
	
	buffer_count--;
	*(line_buffer+buffer_count) = '\0';
}

/* 
 * buffer_command()
 *   Description: Calls a command based off of what's in the line buffer
 *         Input: None
 *        Output: None
 *        Return: None
 *  Side Effects: Calls a command after scanning the line buffer and clears it afterwards
 */
void buffer_command() {
	//printf("Line buffer size is %d", buffer_count-1);
	//putc('\n');
	if (buffer_output_enabled)
		printf("Buffer: \"%s\" (size %d)\n", line_buffer, buffer_count);

	uint8_t i, j, is_arg=0;
	char com_and_arg[2][buffer_count];
	for (i=0; i<2; i++) {
		for (j=0; j<buffer_count; j++) {
			com_and_arg[i][j]='\0';
		}
	}
	j=0;
	for (i=0; i<buffer_count-1; i++) {
		if (is_arg==0 && ( *(line_buffer+i)==' ' || *(line_buffer+i)=='\0') ) {
			com_and_arg[is_arg][j] = '\0';
			is_arg = 1;
			j = 0;
		}
		else {
			com_and_arg[is_arg][j] = *(line_buffer+i);
			j++;
		}
	}
	com_and_arg[1][j] = '\0';

	printf("Command: \"%s\"\n", com_and_arg[0]);
	printf("Argument: \"%s\"\n", com_and_arg[1]);

	if (!strncmp(com_and_arg[0], "buffer", 6)) {
		if (buffer_output_enabled==0) {
			buffer_output_enabled = 1;
			printf("Buffer output enabled\n");
		} else {
			buffer_output_enabled = 0;
			printf("Buffer output disabled\n");
		}
	}

	if (!strncmp(com_and_arg[0], "echo", 4)) {
		printf("%s\n", com_and_arg[1]);
	}

}

/* 
 * clear_command()
 *   Description: Clears the line buffer entirely
 *         Input: None
 *        Output: None
 *        Return: None
 *  Side Effects: Clears the line buffer and resets buffer_count to 0
 */
void clear_buffer() {
	/* if the pointer to the line buffer is NULL, return */
	if (line_buffer == NULL)
		return;
	
	int i;
	for (i = 0; i < buffer_count; i++) {
		*(line_buffer+i) = '\0';
	}
	
	buffer_count = 0;
}
