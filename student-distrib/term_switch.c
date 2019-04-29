/* term_switch.c - terminal switches
 */
 
#include "term_switch.h"
#include "sys_calls.h"
#include "keyboard.h"
#include "paging.h"
#include "lib.h"
#include "term_driver.h"
#include "pit.h"

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
 *	SIDE EFFECTS: set all global variables affiliated with terminal programs to start in terminal 1
 */
void init_terminal(){
	/* initialize data in all terminal structures */
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
	
	/* set the global line buffer and physical video mapping to terminal 1 */
	line_buffer = terminal_array[1].keyboard;
	buffer_count = &terminal_array[1].buf_count;
	map_terminal(1);
}

/*
 *	switch_terminal 
 *
 *	INPUTS: uint8_t keycode - the keycode of the function button pressed
 *	OUTPUTS: none
 *	RETURN VALUE: none
 *	SIDE EFFECTS: remaps the terminal's virtual addresses and transfers the vid memory of old and new terminal
 */
void switch_terminal(uint8_t keycode) {
	/* retrieve the terminal number to switch to, determine whether it's already being displayed, and retrive its virtual address */
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

	/* critical section */
	cli();

	/* transfer video memory between terminal buffers, remap terminal's virtual addresses, and update virtual address of terminal being displayed */
	reset_mapping();
	memcpy((uint32_t*)curr_addr, (uint32_t*)VIDEO_ADDR, KB_4);
	memcpy((uint32_t*)VIDEO_ADDR, (uint32_t*)new_addr, KB_4);
	map_terminal(new_term_num);
	curr_addr = new_addr;

	/* change line buffer pointer to newly displayed terminal */
	line_buffer = terminal_array[new_term_num].keyboard;
	buffer_count = &terminal_array[new_term_num].buf_count;

	/* update the number of currently displayed terminal */
	curr_term_num = new_term_num;
	display_cursor(terminal_array[curr_term_num].screenx, terminal_array[curr_term_num].screeny);

	sti();
}

/*
 *	schedule_terminal (uint32_t old_terminal)
 *
 *	INPUTS: uint32_t old_terminal - terminal the scheduler is leaving
 *	OUTPUTS: none
 *	RETURN VALUE: none
 *	SIDE EFFECTS: remaps vidmap virtual address and sets lib.c video pointer for the next program on the scheduler
 */
void schedule_terminal(uint32_t old_terminal) {
	/* determine which terminal is PIT going to switch to */
	uint32_t new_terminal = old_terminal + 1;
	if(new_terminal==TERM_3+1) new_terminal=1;

	/* set lib.c to point to new virtual terminal address, and if terminal to switch to is the one being displayed,
	*  set mapping of vidmap to physical video memory, otherwise set it to the respective temrinal buffer
	*/
	if(curr_term_num == new_terminal)
	{
		remap_real();
		set_vidmem(new_terminal);
	}
	else
	{
		remap_shadow(new_terminal);
		set_vidmem(new_terminal);
	}
}
