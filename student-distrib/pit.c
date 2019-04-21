#include "pit.h"
#include "lib.h"
#include "sys_calls.h"
#include "i8259.h"
#include "term_switch.h" 
#include "paging.h"

uint32_t PIT_terminal=1;
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
	send_eoi(PIT_IRQ);
	
	uint32_t new_terminal = PIT_terminal + 1;
	if(new_terminal==4) new_terminal=1;
	
	
	//save esp ebp
    asm volatile("movl %%esp,%%eax;"
        : "=a"(terminal_array[PIT_terminal].esp)
        :
        : "memory");
    asm volatile("movl %%ebp,%%eax;" 
    : "=a"(terminal_array[PIT_terminal].ebp)
    :
    : "memory");

	schedule_terminal(PIT_terminal);
	if(first_rotation==TERM_1 ||first_rotation==TERM_2|| first_rotation==TERM_3){
		uint32_t pid = first_rotation;
		first_rotation++; PIT_terminal=new_terminal; 
		current_pid = 0;

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
		//restore esp ebp of new terminal
		PIT_terminal = new_terminal;
		current_pid = terminal_array[PIT_terminal].curr_pid;		

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
