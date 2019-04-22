# keyboard_asm.S - Assembly linkeage that connects interrupt handler with keyboard handler
# vim:ts=4 noexpandtab

.text

.globl keyboard_handler
.globl test_cr2

/* 
 * keyboard_handler()
 *   Description: Calls keyboard_handler_function at keyboard.h
 *         Input: None
 *        Output: None
 *        Return: None
 *  Side Effects: Saves all registers and flags, then calls the keyboard handler function,
 *                restores registers, then return from interrupt 
 */
keyboard_handler:
    # saving flags
    pushal
    pushfl
    call keyboard_handler_function
    popfl
    popal
    iret


test_cr2:
    movl %cr2,%eax
    ret