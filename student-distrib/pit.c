#include "pit.h"
#include "lib.h"
#include "sys_calls.h"
#include "i8259.h"

/*
 * init_pit
 *   DESCRIPTION: Initializes the PIT to proper values.
 *   INPUTS: NONE
 *   OUTPUTS: NONE
 *   RETURN VALUE: NONE
 *   SIDE EFFECTS: PIT is initialized
 */
void init_pit(){
	outb(PIT_MODE, PIT_PORT);
	outb(PIT_FREQ & PIT_MASK, CHANNEL0);
	outb(PIT_FREQ >> SHIFT_8, CHANNEL0);
	enable_irq(PIT_IRQ);
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
}
