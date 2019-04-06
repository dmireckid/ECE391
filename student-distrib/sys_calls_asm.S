# sys_calls_asm.S - Assembly linkage for system calls

.data
    SYS_CALL_NUM_MIN = 1
    SYS_CALL_NUM_MAX = 10

.text

.globl sys_call_handler
.globl context_switch, after_iret


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
	
	# push the registers
	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %edx
    pushl %ecx
	pushl %ebx
	# push the arguments
	pushl %edx
    pushl %ecx
	pushl %ebx
	# enable interrupts
	sti

    call *jump_table(,%eax,4)
	
	# disable interrupts
	cli
	# pop the arguments
	addl $12, %esp
	# pop the registers
	popl %ebx
	popl %ecx
	popl %edx
	popl %esi
	popl %edi
	popl %ebp
	
	IRET

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
 *        Inputs: user_ds - value of USER_DS
 *				  iret_esp - 132MB-1
 *				  user_cs - value of USER_CS
 *				  entry - entry point
 *        Output: None
 *        Return: None
 *  Side Effects: context is switched
 */
context_switch:
	# push USER_DS
	pushl 4(%esp)
	# push IRET ESP
	pushl 12(%esp)
	# push flags
	pushfl
	# make IF to be 1
	popl %eax
	orl $0x200, %eax
	pushl %eax
	# push USER_CS
	pushl 24(%esp)
	# push entry point
	pushl 32(%esp)
	IRET

after_iret:
	leave
	ret

jump_table:
    .long 0, halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn
