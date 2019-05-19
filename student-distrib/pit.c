#include "pit.h"
#include "lib.h"
#include "sys_calls.h"
#include "i8259.h"
#include "term_switch.h" 
#include "paging.h"
#include "x86_desc.h"

uint32_t PIT_terminal=TERM_3;
uint32_t first_rotation = 1;


/*
 * init_pit
 *   DESCRIPTION: Initializes the PIT to proper values.
 *   INPUTS: NONE
 *   OUTPUTS: NONE
 *   RETURN VALUE: NONE
 *   SIDE EFFECTS: PIT is initialized
 */
void init_pit(){
	outb(PIT_MODE, PIT_PORT);	//set pit to square wave counter
	outb(PIT_FREQ & PIT_MASK, CHANNEL0); //lower bits to PIT's channel 0
	outb(PIT_FREQ >> SHIFT_8, CHANNEL0); //higher bits to PIT's channel 0
	enable_irq(PIT_IRQ);	//enable PIT's IRQ to allow for interrupts
}

/* 
 * pit_handler_function()
 *   Description: The PIT interrupt handler for scheduling
 *         Input: None
 *        Output: None
 *        Return: None
 *  Side Effects: Sends EOI to PIT_IRQ and switches to another process through a round-robin scheduler
 */
void pit_handler_function(){
	
	//send an EOI to allow other interrupts to occur
	send_eoi(PIT_IRQ);

	//create the new terminal number, which cycles between 1-3
	uint32_t new_terminal = PIT_terminal + 1;
	if(new_terminal==TERM_3+1) new_terminal=1;


	//save esp/ebp of current terminal
    asm volatile("movl %%esp,%%eax;"
        : "=a"(terminal_array[PIT_terminal].esp)
        :
        : "memory");
    asm volatile("movl %%ebp,%%eax;" 
    : "=a"(terminal_array[PIT_terminal].ebp)
    :
    : "memory");

	//set the video paging so that it points to the correct terminal buffer/display
	schedule_terminal(PIT_terminal);


	/* if the PIT interrupt is one of the first three when the system's booted up, boot up a base shell instead */
	if(first_rotation==TERM_1 ||first_rotation==TERM_2|| first_rotation==TERM_3){

		// store the current cursor in the terminal being left and update it to the terminal being switched to
		terminal_array[PIT_terminal].screenx = screen_x;
		terminal_array[PIT_terminal].screeny = screen_y;
		screen_x = terminal_array[new_terminal].screenx;
		screen_y = terminal_array[new_terminal].screeny;

		// clear the screen when starting up the base shell for the first time
		ctrl_l();

		// set the PID for the base shell of the terminal (Terminal 1 Base Shell is at Index 1 of PCB array, etc.)
		uint8_t pid = new_terminal;
		terminal_array[new_terminal].curr_pid = pid;
		first_rotation++; PIT_terminal=new_terminal; 

		// set the parent 
		uint32_t kernel_stack_bottom = MB_8 - pid*KB_8;

		asm volatile(
		"movl %0,%%esp;"
		"movl %1, %%ebp;"
        : 
        : "r"(kernel_stack_bottom ), "r"(kernel_stack_bottom )
        : "memory");


		execute((uint8_t*)"shell");
	}
	else{

		// store the current cursor in the terminal being left and update it to the terminal being switched to
		terminal_array[PIT_terminal].screenx = screen_x;
		terminal_array[PIT_terminal].screeny = screen_y;
		screen_x = terminal_array[new_terminal].screenx;
		screen_y = terminal_array[new_terminal].screeny;

		//if the PIT is working on the currently displayed terminal, update the cursor
		if(new_terminal == curr_term_num) display_cursor(screen_x,screen_y);

		//set PIT_terminal to hold the new terminal number
		PIT_terminal = new_terminal;
		//switch process paging
		remap_page(terminal_array[PIT_terminal].curr_pid);	
		//set tss
		tss.esp0 = MB_8 - terminal_array[PIT_terminal].curr_pid*KB_8;
		//restore esp ebp of new terminal
		asm volatile(
		"movl %0,%%esp;"
		"movl %1, %%ebp;"
		"jmp after_iret"
        : 
        : "r"(terminal_array[new_terminal].esp), "r"(terminal_array[new_terminal].ebp)
        : "memory");
		return;
	}
}
