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

static zero_arg rtc_jumptable[4] = {(zero_arg)rtc_open,(zero_arg)rtc_read,(zero_arg)rtc_write,(zero_arg)rtc_close};
static zero_arg terminal_jumptable[4] = {(zero_arg)terminal_open,(zero_arg)terminal_read,(zero_arg)terminal_write,(zero_arg)terminal_close};
static zero_arg directory_jumptable[4] = {(zero_arg)open_d,(zero_arg)read_d,(zero_arg)write_d,(zero_arg)close_d};
static zero_arg file_jumptable[4] = {(zero_arg)open_f,(zero_arg)read_f,(zero_arg)write_f,(zero_arg)close_f};

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
 *	INPUTS:
 *  const uint8_t* command - a string specifying a program
 *	OUTPUTS: none
 *	RETURN VALUE: the execute call returns -1 if the command cannot be executed,
 *  for example, if the program does not exist or the filename specified is not an executable, 256 if the program dies by an
 *  exception, or a value in the range 0 to 255 if the program executes a halt system call, in which case the value returned
 *  is that given by the program’s call to halt.
 *	SIDE EFFECTS: RTC may run faster 
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

    //Initialize PCB stdin/out file descriptors
    init_STD(current_pid);
    pcb_array[current_pid].parent_pid = current_pid-1;
    
    asm volatile("movl %%esp,%0" : "=rm"(pcb_array[current_pid].parent_kernel_esp));
    asm volatile("movl %%ebp,%0" : "=rm"(pcb_array[current_pid].parent_kernel_ebp));
    //pcb_array[current_pid].parent_kernel_esp = 
    //pcb_array[current_pid].parent_kernel_ebp =
    
    tss.esp0 =MB_8 - current_pid*KB_8;

    //get entry point


    return 0;
}

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

}


int32_t read (int32_t fd, void* buf, int32_t nbytes)
{
    return 0;
}
int32_t write (int32_t fd, const void* buf, int32_t nbytes)
{
    return 0;
}
int32_t open (const uint8_t* filename)
{
    return 0;
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
