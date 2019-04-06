# sys_calls_asm.S - Assembly linkage for system calls

.data
    SYS_CALL_NUM_MIN =	1
    SYS_CALL_NUM_MAX =	10
	POP_12			 =	12
	ABNORMAL		 =	3
	GET_USER_DS		 =	4
	GET_IRET_ESP	 =	12
	IF_MASK			 =	0x200
	GET_USER_CS		 =	24
	GET_ENTRY_POINT	 =	32

.text

.globl sys_call_handler
.globl context_switch, after_iret


/* 
 * sys_call_handler
 *   Description: dispatcher function
 *        Inputs: %eax,%ebx,%ecx,%edx
 *        Output: None
 *        Return: None
 *  Side Effects: 
 */
sys_call_handler:
	# check to see that the sys_call number is in bounds
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
	addl $POP_12, %esp
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
 *   Description: returns 3 due to an invalid sys_call number
 *        Inputs: None
 *        Output: None
 *        Return: None
 *  Side Effects: 
 */
sys_call_invalid:
    movl $ABNORMAL,%eax
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

/* 
 * after_iret
 *   Description: returns execute through halt
 *        Inputs: None
 *        Output: None
 *        Return: None
 *  Side Effects: returns execute
 */
after_iret:
	leave
	ret

# jump table for system call C functions
jump_table:
    .long 0, halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn
