/* term_switch.c - terminal switches
 */
 
#include "term_switch.h"
#include "sys_calls.h"
#include "keyboard.h"
#include "paging.h"
#include "lib.h"
#include "term_driver.h"

uint8_t term1pid=SHELL1;
uint8_t term2pid=SHELL2;
uint8_t term3pid=SHELL3;

static term_t term1;
static term_t term2;
static term_t term3;

uint8_t curr_term_num=SHELL1;
term_t* curr_term_struct=&term1;
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
	line_buffer = term1.keyboard;
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
	
	term_t* new_term;
	uint32_t new_addr;
	switch (keycode) {
		case (F1_P):
			if (curr_term_num == SHELL1)
				return;
			new_addr = TERM_VID_1;
			new_term = &term1;
			break;
		case (F2_P):
			if (curr_term_num == SHELL2)
				return;
			new_addr = TERM_VID_2;
			new_term = &term2;
			break;
		case (F3_P):
			if (curr_term_num == SHELL3)
				return;
			new_addr = TERM_VID_3;
			new_term = &term3;
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
	curr_term_struct->buf_count = buffer_count;
	curr_term_struct->screenx = screen_x;
	curr_term_struct->screeny = screen_y;

	/* move keyboard stuff from vid memory of terminal being entered to kernel vid memory */
	line_buffer = new_term->keyboard;
	buffer_count = new_term->buf_count;
	screen_x = new_term->screenx;
	screen_y = new_term->screeny;
	update_cursor(screen_x, screen_y);

	switch (keycode) {
		case (F1_P):
			curr_term_struct = &term1;
			break;
		case (F2_P):
			curr_term_struct = &term2;
			break;
		case (F3_P):
			curr_term_struct = &term3;
			break;
		default:
			return;
	}

}

void backstage(uint8_t term_num) {
	
}
