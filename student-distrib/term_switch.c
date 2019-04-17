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

uint8_t curr_term=SHELL1;
uint32_t curr_addr=TERM_VID_1;

void switch_terminal(uint8_t keycode) {
	
	uint32_t load_addr;
	switch (keycode) {
		case (F1_P):
			if (curr_term == SHELL1)
				return;
			load_addr = TERM_VID_1;
			break;
		case (F2_P):
			if (curr_term == SHELL2)
				return;
			load_addr = TERM_VID_2;
			break;
		case (F3_P):
			if (curr_term == SHELL3)
				return;
			load_addr = TERM_VID_3;
			break;
		default:
			return;
	}
	
	memcpy((uint32_t*)curr_addr, (uint32_t*)VIDEO_ADDR, KB_4);
	memcpy((uint32_t*)VIDEO_ADDR, (uint32_t*)load_addr, KB_4);
	curr_addr = load_addr;
	curr_term = keycode-F1_P+1;
}
