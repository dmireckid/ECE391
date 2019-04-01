# keyboard_asm.S - Assembly linkage for system calls


.data
    SYS_CALL_NUM_MIN = 1
    SYS_CALL_NUM_MAX = 10

.text

.globl sys_call_handler,sys_call_halt, sys_call_execute, sys_call_read, sys_call_write,  sys_call_open,  sys_call_close, sys_call_getargs, sys_call_vidmap, sys_call_sethandler, sys_call_sigreturn

/* 
 * sys_call_handler
 *   Description: dispatcher function
 *         Inputs: %eax,%ebx,%ecx,%edx
 *        Output: None
 *        Return: None
 *  Side Effects: 
 */
sys_call_handler:
    cmpl $SYS_CALL_NUM_MIN,%eax
    jb sys_call_invalid

    cmpl $SYS_CALL_NUM_MAX,%eax
    ja sys_call_invalid

    jmp *jump_table(,%eax,4);

sys_call_invalid:
    movl $-1,%eax
    IRET

sys_call_open:
	# push arguments and call open
    pushl %ebx
    call open
    popl %ebx
    IRET

sys_call_close:

sys_call_read:
	# push arguments and call read
    pushl %edx
    pushl %ecx
	pushl %ebx
    call halt
    popl %ebx
    popl %ecx
    popl %edx

sys_call_write:

sys_call_halt:
	# push arguments and call halt
    pushl %ebx
    call halt
    popl %ebx

sys_call_execute:
	# push arguments and call execute
    pushl %ebx
    call execute
    popl %ebx
	# set up entire IRET stack





sys_call_getargs:


sys_call_vidmap:


sys_call_sethandler:


sys_call_sigreturn:


jump_table:
    .long 0,sys_call_halt, sys_call_execute, sys_call_read, sys_call_write,  sys_call_open,  sys_call_close, sys_call_getargs, sys_call_vidmap, sys_call_sethandler, sys_call_sigreturn