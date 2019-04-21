/* term_switch.c - terminal switches
 */
 
#include "term_switch.h"
#include "sys_calls.h"
#include "keyboard.h"
#include "paging.h"
#include "lib.h"
#include "term_driver.h"

uint8_t term1pid = TERM_1;
uint8_t term2pid = TERM_2;
uint8_t term3pid = TERM_3;

uint8_t curr_term_num = TERM_1;
uint8_t curr_term_struct = TERM_1;
uint32_t curr_addr=TERM_VID_1;

/*
 *	init_terminal
 *
 *	INPUTS: none
 *	OUTPUTS: none
 *	RETURN VALUE: none
 *	SIDE EFFECTS: set the global line buffer pointer to the buffer in terminal 1
 */
void init_terminal(){
	line_buffer = terminal_array[1].keyboard;
}

/*
 *	switch_terminal 
 *
 *	INPUTS: uint8_t keycode - the keycode of the function button pressed
 *	OUTPUTS: none
 *	RETURN VALUE: none
 *	SIDE EFFECTS: transfers the vid memory of new terminal and stores the vid memory of the terminal being left
 */
void switch_terminal(uint8_t keycode) {
	
	uint8_t new_term = keycode-F1_P+1;
	uint32_t new_addr;
	switch (keycode) {
		case (F1_P):
			if (curr_term_num == TERM_1)
				return;
			new_addr = TERM_VID_1;
			break;
		case (F2_P):
			if (curr_term_num == TERM_2)
				return;
			new_addr = TERM_VID_2;
			break;
		case (F3_P):
			if (curr_term_num == TERM_3)
				return;
			new_addr = TERM_VID_3;
			break;
		default:
			return;
	}
	
	/* transfer video memory */
	memcpy((uint32_t*)curr_addr, (uint32_t*)VIDEO_ADDR, KB_4);
	memcpy((uint32_t*)VIDEO_ADDR, (uint32_t*)new_addr, KB_4);
	curr_addr = new_addr;
	curr_term_num = keycode-F1_P+1;
	
	/* store keyboard stuff from kernel vid memory to vid memory of terminal being left */
	terminal_array[curr_term_struct].buf_count = buffer_count;
	terminal_array[curr_term_struct].screenx = screen_x;
	terminal_array[curr_term_struct].screeny = screen_y;

	/* move keyboard stuff from vid memory of terminal being entered to kernel vid memory */
	line_buffer = terminal_array[new_term].keyboard;
	buffer_count = terminal_array[new_term].buf_count;
	screen_x = terminal_array[new_term].screenx;
	screen_y = terminal_array[new_term].screeny;
	update_cursor(screen_x, screen_y);

	curr_term_struct = new_term;

}

void backstage(uint8_t term_num) {
	
}
