/* term_switch.c - terminal switches
 */
 
#include "term_switch.h"
#include "sys_calls.h"
#include "keyboard.h"
#include "paging.h"
#include "lib.h"

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
 *	switch_terminal 
 *
 *	INPUTS: uint8_t keycode - the keycode of the function button pressed
 *	OUTPUTS: none
 *	RETURN VALUE: none
 *	SIDE EFFECTS: transfers the vid memory of new terminal and stores the vid memory of the terminal being left
 */
void switch_terminal(uint8_t keycode) {
	
	term_t* load_term;
	uint32_t load_addr;
	switch (keycode) {
		case (F1_P):
			if (curr_term_num == SHELL1)
				return;
			load_addr = TERM_VID_1;
			load_term = &term1;
			break;
		case (F2_P):
			if (curr_term_num == SHELL2)
				return;
			load_addr = TERM_VID_2;
			load_term = &term2;
			break;
		case (F3_P):
			if (curr_term_num == SHELL3)
				return;
			load_addr = TERM_VID_3;
			load_term = &term3;
			break;
		default:
			return;
	}
	
	/* transfer video memory */
	memcpy((uint32_t*)curr_addr, (uint32_t*)VIDEO_ADDR, KB_4);
	memcpy((uint32_t*)VIDEO_ADDR, (uint32_t*)load_addr, KB_4);
	curr_addr = load_addr;
	curr_term_num = keycode-F1_P+1;
	
	/* store keyboard stuff from kernel vid memory to vid memory of terminal being left */
	memcpy(curr_term_struct->keyboard,line_buffer,LINE_BUFFER_SIZE);
	curr_term_struct->buf_count = buffer_count;
	curr_term_struct->screenx = screen_x;
	curr_term_struct->screeny = screen_y;

	/* move keyboard stuff from vid memory of terminal being entered to kernel vid memory */
	memcpy(line_buffer,load_term->keyboard,LINE_BUFFER_SIZE);
	buffer_count = load_term->buf_count;
	screen_x = load_term->screenx;
	screen_y = load_term->screeny;
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
