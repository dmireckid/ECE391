# paging_assem.S - Enable paging

.text

.globl enable_paging

# enable paging by properly setting the control register values
enable_paging:
	# store the pointer of the page directory in CR3
	push %ebp
	mov %esp, %ebp
	mov 8(%esp), %eax
	mov %eax, %cr3
	mov %ebp, %esp
	pop %ebp
	# set bit 32 in CR0 to 1 to enable paging
	push %ebp
	mov %esp, %ebp
	mov %cr0, %eax
	or $0x80000000, %eax
	mov %eax, %cr0
	mov %ebp, %esp
	pop %ebp
	ret
