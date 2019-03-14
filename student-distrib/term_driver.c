#include "term_driver.h"
#include "keyboard.h"

int buffer_count = 0;

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
	putc('\n');
	printf("Line buffer size is %d", buffer_count-1);
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
	for (i = 0; i < LINE_BUFFER_SIZE; i++) {
		*(line_buffer+buffer_count) = '\0';
	}
	
	buffer_count = 0;
}
