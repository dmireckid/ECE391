/* rtc_asm.h - assembly linkage for rtc interrupts
 */



#ifndef _RTC_ASM_H
#define _RTC_ASM_H


/*
 * rtc_handler()
 *   DESCRIPTION: this is the interrupt handler for rtc interrupts
 *   INPUTS: none
 *   OUTPUTS: nones
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Saves all registers and flags, then calls the rtc_interrupt,
 *                restores registers, then returns from interrupt 
 */
extern void rtc_handler();

#endif
