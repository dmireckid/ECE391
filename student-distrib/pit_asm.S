.text

.globl pit_handler

/* 
 * pit_handler()
 *   Description: Calls pit_handler_function at pit.h
 *         Input: None
 *        Output: None
 *        Return: None
 *  Side Effects: Saves all registers and flags, then calls the pit handler function,
 *                restores registers, then return from interrupt 
 */
 
pit_handler:
    # saving registers and flags
    pushal
    pushfl
	# call the actual function
    call pit_handler_function
    popfl
    popal
    iret
