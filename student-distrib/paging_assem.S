# paging_assem.S - Enable paging

.text

.globl enable_paging

# enable paging by properly setting the control register values
enable_paging:
	# store the pointer of the page directory in CR3
	pushl %ebp
	movl %esp, %ebp
	movl 8(%esp), %eax
	movl %eax, %cr3
	# enable the 4MB page extension in CR4
	movl %cr4, %eax
	orl $0x10, %eax
	movl %eax, %cr4
	# set bit 0 (PE) and bit 31 (PG) in CR0 to 1 to enable paging
	movl %cr0, %eax
	orl $0x80000001, %eax
	movl %eax, %cr0
	movl %ebp, %esp
	popl %ebp
	ret
