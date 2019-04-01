/* sys_calls.c - system calls
 */

#include "sys_calls.h"
#include "rtc.h"
#include "term_driver.h"
#include "keyboard.h"
#include "filesys.h"
#include "x86_desc.h"
#include "lib.h"
#include "paging.h"

static uint32_t rtc_jumptable[4] = { (uint32_t)&rtc_open,(uint32_t)&rtc_read,(uint32_t)&rtc_write,(uint32_t)&rtc_close};
static uint32_t terminal_jumptable[4] = {(uint32_t)&terminal_open,(uint32_t)&terminal_read,(uint32_t)&terminal_write,(uint32_t)&terminal_close};
static uint32_t directory_jumptable[4] = {(uint32_t)&open_d,(uint32_t)&read_d,(uint32_t)&write_d,(uint32_t)&close_d};
static uint32_t file_jumptable[4] = {(uint32_t)&open_f,(uint32_t)&read_f,(uint32_t)&write_f,(uint32_t)&close_f};

#define ELF_SIZE 4
static int8_t elf_string[ELF_SIZE] = {0x7f,0x45,0x4c,0x4f};

#define MAX_PROCESSES 6
static uint32_t current_pid=-1;
static pcb_t pcb_array[MAX_PROCESSES];
#define pid_shell_0  0
#define pid_program_0  1
#define PROGRAM_VIRTUAL_ADDRESS 0x800000



int32_t halt (uint8_t status){
    return 0;

}

/*
 *	execute 
 *
 *	INPUTS: const uint8_t* command - a string specifying a program
 *	OUTPUTS: none
 *	RETURN VALUE: the execute call returns -1 if the command cannot be executed,
 *  for example, if the program does not exist or the filename specified is not an executable, 256 if the program dies by an
 *  exception, or a value in the range 0 to 255 if the program executes a halt system call, in which case the value returned
 *  is that given by the program's call to halt.
 *	SIDE EFFECTS: program is added to PCB array
 */
int32_t execute (const uint8_t* command)
{
    dentry_t test;
    int8_t buf[ELF_SIZE];
    int32_t file_size;
    
    //check if file exists
    if(read_dentry_by_name(command,&test)==-1) return -1;
   //check if the filetype is a file
    if(test.filetype !=2) return -1;
    //check if the first 4 bytes are ELF magic number
    file_size = read_data(test.inode_num,0,(uint8_t*) buf,ELF_SIZE);
    if(strncmp(buf,elf_string,ELF_SIZE)!=0) return -1;

    //assign pid and memory for the process
    current_pid++;
    remap_page(current_pid);

    //copy program into memory
    read_data(test.inode_num,0,(uint8_t*)PROGRAM_VIRTUAL_ADDRESS,PROGRAM_SIZE);

    //Initialize PCB values and stdin/out file descriptors
    init_STD(current_pid);
    pcb_array[current_pid].parent_pid = current_pid-1;
    
    asm volatile("movl %%esp,%0" : "=rm"(pcb_array[current_pid].parent_kernel_esp));
    asm volatile("movl %%ebp,%0" : "=rm"(pcb_array[current_pid].parent_kernel_ebp));
    //pcb_array[current_pid].parent_kernel_esp = 
    //pcb_array[current_pid].parent_kernel_ebp =
    
	//set tss values
    tss.esp0 = MB_8 - current_pid*KB_8;
	tss.ss0 = KERNEL_DS;

    //get entry point


    return 0;
}

/*
 *	init_STD
 *
 *	INPUTS: uint32_t pid - the processor ID of the PCB
 *	OUTPUTS: none
 *	RETURN VALUE: none
 *	SIDE EFFECTS: stdin and stdout are respectively initialized in index 0 and 1 of file descriptor array
 */
void init_STD(uint32_t pid)
{
    pcb_array[pid].fd_array[0].fops = (uint32_t)terminal_jumptable;
    pcb_array[pid].fd_array[0].inode =0;
    pcb_array[pid].fd_array[0].fp =0;
    pcb_array[pid].fd_array[0].flags =IN_USE_FLAG;

    pcb_array[pid].fd_array[1].fops = (uint32_t)terminal_jumptable;
    pcb_array[pid].fd_array[1].inode =0;
    pcb_array[pid].fd_array[1].fp =0;
    pcb_array[pid].fd_array[1].flags =IN_USE_FLAG;

	int i;
	for (i = 2; i < 8; i++) {
		pcb_array[pid].fd_array[1].flags = NOT_IN_USE_FLAG;
	}

    pcb_array[pid].files_in_use = 2;
}

/*
 *	read
 *
 *	INPUTS: int32_t fd - file descriptor
 * 			void* buf - buffer
 *			int32_t nbytes - file size (in bytes)
 *	OUTPUTS: none
 *	RETURN VALUE: number of bytes written, or -1 on failure
 *	SIDE EFFECTS: runs the read command based on the file type
 */
int32_t read (int32_t fd, void* buf, int32_t nbytes)
{
    //check if file descriptor is in bounds and if the flag is IN_USE
    if(fd > 7 || fd < 0) return -1;
	if(pcb_array[current_pid].fd_array[fd].flags == NOT_IN_USE_FLAG) return -1;
 
    uint32_t* ptr = (uint32_t*)pcb_array[current_pid].fd_array[fd].fops; 
    int32_t (*fun_ptr)(int32_t, const void*, int32_t) = (void*)ptr[1];
    return (*fun_ptr)(fd,buf,nbytes);
}

/*
 *	write
 *
 *	INPUTS: int32_t fd - file descriptor
 * 			void* buf - buffer
 *			int32_t nbytes - file size (in bytes)
 *	OUTPUTS: none
 *	RETURN VALUE: number of bytes written, or -1 on failure
 *	SIDE EFFECTS: runs the write command based on the file type
 */
int32_t write (int32_t fd, const void* buf, int32_t nbytes)
{
    //check if file descriptor is in bounds and if the flag is IN_USE
    if(fd > 7 || fd < 0) return -1;
	if(pcb_array[current_pid].fd_array[fd].flags == NOT_IN_USE_FLAG) return -1;
 
    uint32_t* ptr = (uint32_t*)pcb_array[current_pid].fd_array[fd].fops; 
    int32_t (*fun_ptr)(int32_t, const void*, int32_t) = (void*)ptr[2];
    return (*fun_ptr)(fd,buf,nbytes);
}

/*
 *	open
 *
 *	INPUTS: const uint8_t* filename - pointer to file name
 *	OUTPUTS: none
 *	RETURN VALUE: file descriptor value
 *	SIDE EFFECTS: runs the open command based on the file type
 */
int32_t open (const uint8_t* filename)
{
    dentry_t test;
    //check if file exists
    if(read_dentry_by_name(filename,&test)==-1) return -1;
    switch(test.filetype)
    {
        case 0://rtc
        {
            pcb_array[current_pid].files_in_use++;
            pcb_array[current_pid].fd_array[pcb_array[current_pid].files_in_use-1].fops = (uint32_t)rtc_jumptable;
            pcb_array[current_pid].fd_array[pcb_array[current_pid].files_in_use-1].inode = 0;
            pcb_array[current_pid].fd_array[pcb_array[current_pid].files_in_use-1].fp = 0;
            pcb_array[current_pid].fd_array[pcb_array[current_pid].files_in_use-1].flags = IN_USE_FLAG;
            break;
        }
        case 1://directory
        {

            pcb_array[current_pid].files_in_use++;
            pcb_array[current_pid].fd_array[pcb_array[current_pid].files_in_use-1].fops = (uint32_t)directory_jumptable;
            pcb_array[current_pid].fd_array[pcb_array[current_pid].files_in_use-1].inode = 0;
            pcb_array[current_pid].fd_array[pcb_array[current_pid].files_in_use-1].fp = 0;
            pcb_array[current_pid].fd_array[pcb_array[current_pid].files_in_use-1].flags = IN_USE_FLAG;
            break;
        }
        case 2://file
        {

            pcb_array[current_pid].files_in_use++;
            pcb_array[current_pid].fd_array[pcb_array[current_pid].files_in_use-1].fops = (uint32_t)file_jumptable;
            pcb_array[current_pid].fd_array[pcb_array[current_pid].files_in_use-1].inode = test.inode_num;
            pcb_array[current_pid].fd_array[pcb_array[current_pid].files_in_use-1].fp = 0;
            pcb_array[current_pid].fd_array[pcb_array[current_pid].files_in_use-1].flags = IN_USE_FLAG;
            break;
        }
        default:
            return -1;


    }
 
    uint32_t* ptr = (uint32_t*)pcb_array[current_pid].fd_array[pcb_array[current_pid].files_in_use-1].fops; 
    int32_t (*fun_ptr)(const uint8_t*) = (void*)ptr[0];
    (*fun_ptr)(filename);

    
    return pcb_array[current_pid].files_in_use-1;
}

int32_t close (int32_t fd)
{
    return 0;
}

int32_t getargs (uint8_t* buf, int32_t nbytes)
{
    return 0;
}

int32_t vidmap (uint8_t** screen_start)
{
    return 0;
}

int32_t set_handler (int32_t signum, void* handler_address)
{
    return 0;
}

int32_t sigreturn (void)
{
    return 0;
}
