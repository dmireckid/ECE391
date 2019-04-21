#include "term_driver.h"
#include "keyboard.h"
#include "types.h"
#include "lib.h"
#include "term_switch.h"
#include "pit.h"

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
	*buffer_count++;
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
	printf("Number of characters in line buffer before enter was pressed is %d\n", buffer_count-1);
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
	if (terminal_array[PIT_terminal].keyboard == NULL)
		return;
	
	int i;
	for (i = 0; i < LINE_BUFFER_SIZE; i++) {
		*(terminal_array[PIT_terminal].keyboard+i) = '\0';
	}
	
	terminal_array[PIT_terminal].buf_count = 0;
}

/*
 *	terminal_read
 *  DESCRIPTION: copies the line buffer to a given address after the user hits enter
 *	INPUTS: 
 *  int32_t fd - a file descriptor (not used)
 *  const void* buf  - a char* string that has 128 bytes
 *  int32_t nbytes - must be larger than 128 bytes (specifies size of the buffer)
 *	OUTPUTS: writes to the address pointed to by buf
 *	RETURN VALUE:the number of bytes written, or -1 on failure
 *	SIDE EFFECTS: none
 */

int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes)
{
	if(nbytes < LINE_BUFFER_SIZE-1 || buf==NULL) return -1;

	/* int8_t* strcpy(int8_t* dest, const int8_t* src, uint32_t n)
	* Inputs:      int8_t* dest = destination string of copy
	*         const int8_t* src = source string of copy
	*                uint32_t n = number of bytes to copy
	* Return Value: pointer to dest
	* Function: copy n bytes of the source string into the destination string */
	
	while(terminal_array[PIT_terminal].keyboard[terminal_array[PIT_terminal].buf_count-1]!='\n');
		uint32_t count = buffer_count;
		strncpy((int8_t*) buf, line_buffer, buffer_count);
	clear_buffer();
	return count;


}

/*
 *	keyboard_read
 *  DESCRIPTION: copies the line buffer to a given address after the user hits enter
 *	INPUTS: 
 *  int32_t fd - a file descriptor (not used)
 *  const void* buf  - a char* string that has 128 bytes
 *  int32_t nbytes -  must be larger than 128 bytes (specifies size of the buffer)
 *	OUTPUTS: writes to the address pointed to by buf
 *	RETURN VALUE:the number of bytes read, or -1 on failure
 *	SIDE EFFECTS: none
 */

int32_t keyboard_read(int32_t fd, void* buf, int32_t nbytes)
{
	if(nbytes < LINE_BUFFER_SIZE || buf==NULL) return -1;

	/* int8_t* strcpy(int8_t* dest, const int8_t* src, uint32_t n)
	* Inputs:      int8_t* dest = destination string of copy
	*         const int8_t* src = source string of copy
	*                uint32_t n = number of bytes to copy
	* Return Value: pointer to dest
	* Function: copy n bytes of the source string into the destination string */
	
	while(line_buffer[buffer_count-1]!='\n') {
		strncpy((int8_t*) buf, line_buffer, buffer_count);
	}
	//clear_buffer();

	return buffer_count;


}


/*
 *	terminal_write
 *  DESCRIPTION:write a string to the terminal
 * 
 *  
 *	INPUTS:
 *  int32_t fd - file descriptor (not used)
 *  const void* buf - a pointer to a string to be written to the terminal
 *  int32_t nbytes - the number of characters in the string (max 128*25)
 *	OUTPUTS: printed string on the terminal
 *	RETURN VALUE:the number of bytes written, or -1 on failure
 *	SIDE EFFECTS: printed string on the terminal
 */
#define terminal_string_size 80*25
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes)
{
	if(buf==NULL || nbytes<1 || nbytes>terminal_string_size) return -1;
	//clear_buffer();
	int i;

	char* string = (char *) buf;
	for(i=0;i<nbytes;i++)
		putc(*(string+i));
	

	return nbytes;


}


/*
 *	terminal_open
 *  DESCRIPTION: not used
 *	INPUTS:the name of a file , represented as a array of bytes, with max size of 32 (addresses in filesys.c)
 *	OUTPUTS: none
 *	RETURN VALUE:file descriptor 
 *  If the named file does not exist or no descriptors are free, the call returns -1
 *	SIDE EFFECTS: none
 */
int32_t terminal_open(const uint8_t* filename){
	 
	return -1;

 }

 /*
 *	terminal_close
 *  DESCRIPTION:closes the specified file descriptor
 *	INPUTS:a integer that is the file descriptor
 *	OUTPUTS: none
 *	RETURN VALUE:Trying to close an invalid descriptor should result in a return value of -1; successful closes should return 0.
 *	SIDE EFFECTS: none
 */
 int32_t terminal_close(int32_t fd){


	return -1;
}

