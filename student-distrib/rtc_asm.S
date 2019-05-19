#  rtc_asm.S  assembly linkage for rtc interrupts
#define ASM 1


.text

.globl rtc_handler



rtc_handler:
    # should we save flags?
	
	pushal
	pushfl
    call rtc_interrupt
	popfl
	popal
    iret


