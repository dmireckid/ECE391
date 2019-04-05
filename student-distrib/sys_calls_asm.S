# sys_calls_asm.S - Assembly linkage for system calls

.data
    SYS_CALL_NUM_MIN = 1
    SYS_CALL_NUM_MAX = 10

.text

.globl sys_call_handler,sys_call_halt, sys_call_execute, sys_call_read, sys_call_write, sys_call_open, sys_call_close, sys_call_getargs, sys_call_vidmap, sys_call_sethandler, sys_call_sigreturn
.globl context_switch


entry_point:
    .long 0

/* 
 * sys_call_handler
 *   Description: dispatcher function
 *        Inputs: %eax,%ebx,%ecx,%edx
 *        Output: None
 *        Return: None
 *  Side Effects: 
 */
sys_call_handler:
    cmpl $SYS_CALL_NUM_MIN,%eax
    jb sys_call_invalid

    cmpl $SYS_CALL_NUM_MAX,%eax
    ja sys_call_invalid

    jmp *jump_table(,%eax,4)

/* 
 * sys_call_invalid
 *   Description: returns -1 due to invalid
 *        Inputs: None
 *        Output: None
 *        Return: None
 *  Side Effects: 
 */
sys_call_invalid:
    movl $-1,%eax
    IRET

/* 
 * context_switch
 *   Description: does a context switch after execute
 *        Inputs: 
 *        Output: None
 *        Return: None
 *  Side Effects: context is switched
 */
context_switch:

/* 
 * sys_call_halt
 *   Description: runs the halt system call
 *        Inputs: %ebx
 *        Output: None
 *        Return: None
 *  Side Effects: 
 */
sys_call_halt:
	# push arguments and call halt
    pushl %ebx
    call halt
    popl %ebx

/* 
 * sys_call_execute
 *   Description: runs the execute system call
 *        Inputs: %ebx
 *        Output: None
 *        Return: None
 *  Side Effects: 
 */
sys_call_execute:
	# push arguments and call execute
    pushl %ebx
    call execute
    popl %ebx

/* 
 * sys_call_read
 *   Description: runs the read system call
 *        Inputs: %ebx, %ecx, %edx
 *        Output: None
 *        Return: None
 *  Side Effects: 
 */
sys_call_read:
	# push arguments and call read
    pushl %edx
    pushl %ecx
	pushl %ebx
    call read
    popl %ebx
    popl %ecx
    popl %edx
	IRET

/* 
 * sys_call_write
 *   Description: runs the write system call
 *        Inputs: %ebx, %ecx, %edx
 *        Output: None
 *        Return: None
 *  Side Effects: 
 */
sys_call_write:
	# push arguments and call write
    pushl %edx
    pushl %ecx
	pushl %ebx
    call write
    popl %ebx
    popl %ecx
    popl %edx
	IRET

/* 
 * sys_call_open
 *   Description: runs the open system call
 *        Inputs: %ebx
 *        Output: None
 *        Return: None
 *  Side Effects: 
 */
sys_call_open:
	# push arguments and call open
    pushl %ebx
    call open
    popl %ebx
    IRET

/* 
 * sys_call_close
 *   Description: runs the close system call
 *        Inputs: %ebx
 *        Output: None
 *        Return: None
 *  Side Effects: 
 */
sys_call_close:
	# push arguments and call close
    pushl %ebx
    call close
    popl %ebx
    IRET


sys_call_getargs:

sys_call_vidmap:

sys_call_sethandler:

sys_call_sigreturn:


jump_table:
    .long 0, sys_call_halt, sys_call_execute, sys_call_read, sys_call_write, sys_call_open, sys_call_close, sys_call_getargs, sys_call_vidmap, sys_call_sethandler, sys_call_sigreturn