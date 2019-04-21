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

uint32_t curr_term_num = TERM_1;
uint32_t curr_addr = TERM_VID_1;

/*
 *	init_terminal
 *
 *	INPUTS: none
 *	OUTPUTS: none
 *	RETURN VALUE: none
 *	SIDE EFFECTS: set the global line buffer pointer to the buffer in terminal 1
 */
void init_terminal(){
	int i;
	int j;
	for (i = 0; i < TERM_3+1; i++) {
		for (j = 0; j < LINE_BUFFER_SIZE; j++) {
			terminal_array[i].keyboard[j] = '\0';
		}
		terminal_array[i].buf_count = 0;
		terminal_array[i].screenx = 0;
		terminal_array[i].screeny = 0;
	}
	line_buffer = terminal_array[1].keyboard;
	buffer_count = &terminal_array[1].buf_count;
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
	
	uint32_t new_term_num = keycode-F1_P+1;
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
	
	/* store keyboard stuff from kernel vid memory to vid memory of terminal being left */
	terminal_array[curr_term_num].screenx = screen_x;
	terminal_array[curr_term_num].screeny = screen_y;

	/* move keyboard stuff from vid memory of terminal being entered to kernel vid memory */
	line_buffer = terminal_array[new_term_num].keyboard;
	buffer_count = &terminal_array[new_term_num].buf_count;
	screen_x = terminal_array[new_term_num].screenx;
	screen_y = terminal_array[new_term_num].screeny;
	update_cursor(screen_x, screen_y);

	curr_term_num = new_term_num;

}



/*
 *	switch_terminal (uint32_t old_terminal)
 *
 *	INPUTS: uint32_t old_terminal - terminal the scheduler is leaving
 *	OUTPUTS: none
 *	RETURN VALUE: none
 *	SIDE EFFECTS:remaps video memory for the next program on the scheduler
 */
void schedule_terminal(uint32_t old_terminal) {
	
	
	uint32_t new_addr;
	
	uint32_t new_terminal = old_terminal + 1;
	if(new_terminal==4) new_terminal=1;

	if(current_term_num == new_terminal)
	{
		remap_real();set_vidmem(new_terminal);
	}
	else
	{
		remap_shadow(new_terminal);set_vidmem(new_terminal);
	}





}


void backstage(uint32_t term_num) {
	
}
