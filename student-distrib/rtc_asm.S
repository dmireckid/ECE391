#  rtc_asm.S  assembly linkage for rtc interrupts
#define ASM 1

.text

.globl rtc_handler

/* 
 * rtc_handler()
 *   Description: Calls rtc_interrupt at pit.h
 *         Input: None
 *        Output: None
 *        Return: None
 *  Side Effects: Saves all registers and flags, then calls the rtc interrupt function,
 *                restores registers, then return from interrupt 
 */
rtc_handler:
    # should we save flags?
	pushal
	pushfl
    call rtc_interrupt
	popfl
	popal
    iret


