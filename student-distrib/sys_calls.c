/* sys_calls.c - system calls
 */

#include "sys_calls.h"
#include "sys_calls_asm.h"
#include "rtc.h"
#include "term_driver.h"
#include "keyboard.h"
#include "filesys.h"
#include "x86_desc.h"
#include "lib.h"
#include "paging.h"

static uint32_t rtc_jumptable[ELF_SIZE] = { (uint32_t)&rtc_open,(uint32_t)&rtc_read,(uint32_t)&rtc_write,(uint32_t)&rtc_close};
static uint32_t terminal_jumptable[ELF_SIZE] = {(uint32_t)&terminal_open,(uint32_t)&terminal_read,(uint32_t)&terminal_write,(uint32_t)&terminal_close};
static uint32_t directory_jumptable[ELF_SIZE] = {(uint32_t)&open_d,(uint32_t)&read_d,(uint32_t)&write_d,(uint32_t)&close_d};
static uint32_t file_jumptable[ELF_SIZE] = {(uint32_t)&open_f,(uint32_t)&read_f,(uint32_t)&write_f,(uint32_t)&close_f};

static int8_t elf_string[ELF_SIZE] = {ELF_0,ELF_1,ELF_2,ELF_3};

static uint32_t current_pid=0;
//	index 0 will hold the data for kernel.c process, 1-6 will hold the base shell and command programs
static pcb_t pcb_array[MAX_PROCESSES+1];


/*
 *	halt 
 *
 *	INPUTS: uint8_t status - the number corresponding to pid
 *	OUTPUTS: none
 *	RETURN VALUE: the execute call returns after halting the program
 *	SIDE EFFECTS: program is removed from PCB array
 */
int32_t halt (uint8_t status){
    /*    asm volatile("movl %%eax,%%ebp" 
    : 
    :"a"(pcb_array[current_pid].parent_kernel_ebp)
    : "memory");*/
	
	// clear out the fd_array associated with the program being halted
	int i;
	for (i = FILE_TYPE_2; i < MAX_FILES; i++) {
		close(i);
	}

	// write parent process back info to tss.esp0
	tss.esp0 = pcb_array[current_pid].parent_kernel_esp;
	
	// decrement current pid
	current_pid--;
	
	// restore paging
	remap_page(current_pid);

	// set the return value for execute
	uint32_t ret_val;
	if (status == 0)
		ret_val = 0;
	else
		ret_val = 256;

	//move parent esp and ebp values back into esp/ebp registers
    asm volatile(
		"movl %0, %%eax;"
		"movl %1,%%esp;"
		"movl %2, %%ebp;"
		"jmp after_iret"
        : 
        : "r"(ret_val),"r"(pcb_array[current_pid+1].parent_kernel_esp), "r"(pcb_array[current_pid+1].parent_kernel_ebp)
        : "memory");
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
	//check if the max number of processes are running
	if (current_pid == MAX_PROCESSES) return -1;
	
    dentry_t test;
    int8_t buf[ELF_SIZE];
    int32_t file_size;
    
    //check if file exists
    if(read_dentry_by_name(command,&test)==-1) return -1;
	//check if the filetype is a file
    if(test.filetype != FILE_TYPE_2) return -1;
    //check if the first 4 bytes are ELF magic number
    file_size = read_data(test.inode_num,0,(uint8_t*) buf,ELF_SIZE);
    if(strncmp(buf,elf_string,ELF_SIZE)!=0) return -1;

    //assign pid and memory for the process
    current_pid++;
    remap_page(current_pid);
	//uint8_t* te = (uint8_t*)0x08048000;
	//uint32_t blah = *te;

    //copy program into memory
    read_data(test.inode_num,0,(uint8_t*)PROGRAM_VIRTUAL_ADDRESS,PROGRAM_SIZE);

    //Initialize PCB values and stdin/out file descriptors
    init_STD(current_pid);
    pcb_array[current_pid].parent_pid = current_pid-1;
    
	//save parent esp and ebp values
    asm volatile("movl %%esp,%%eax;"
        : "=a"(pcb_array[current_pid].parent_kernel_esp)
        :
        : "memory");
    asm volatile("movl %%ebp,%%eax;" 
    : "=a"(pcb_array[current_pid].parent_kernel_ebp)
    :
    : "memory");


    

	//set tss values
    tss.esp0 = MB_8 - current_pid*KB_8;
	tss.ss0 = KERNEL_DS;

    //get entry point
	uint32_t entry;
	read_data(test.inode_num,INDEX_24,(uint8_t*)&entry,ELF_SIZE);
	//entry += MB_128;
	
	uint32_t user_ds = USER_DS; //store USER_DS in a variable
	uint32_t user_cs = USER_CS; //store USER_CS in a variable
	uint32_t iret_esp = PROGRAM_VIRTUAL_END; //store the IRET esp in a variable
	
	cli();
	context_switch(user_ds, iret_esp, user_cs, entry);

/*    asm volatile(
        "pushl $USER_DS"				//push USER_DS
        "pushl $PROGRAM_VIRTUAL_END"	//push value of 132MB-1
        "pushfl"						//push all flags
        "pushl $USER_CS"				//push USER_CS
        "pushl %%eax"					//push entry point
        "IRET" 
        : 
        : "a"(entry) 
        : "memory"

    );*/
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
	for (i = FILE_TYPE_2; i < MAX_FILES; i++) {
		pcb_array[pid].fd_array[i].flags = NOT_IN_USE_FLAG;
	}


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
    if(fd > MAX_FILES-1 || fd < 0) return -1;
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
    if(fd > MAX_FILES-1 || fd < 0) return -1;
	if(pcb_array[current_pid].fd_array[fd].flags == NOT_IN_USE_FLAG) return -1;
 
    uint32_t* ptr = (uint32_t*)pcb_array[current_pid].fd_array[fd].fops; 
    int32_t (*fun_ptr)(int32_t, const void*, int32_t) = (void*)ptr[FILE_TYPE_2];
    return (*fun_ptr)(fd,buf,nbytes);
}

/*
 *	open
 *
 *	INPUTS: const uint8_t* filename - pointer to file name
 *	OUTPUTS: none
 *	RETURN VALUE: file descriptor value
 * If the named file does not exist or no descriptors are free, the call returns -1.
 *	SIDE EFFECTS: runs the open command based on the file type
 */
int32_t open (const uint8_t* filename)
{
    dentry_t test;
    //check if file exists
    if(read_dentry_by_name(filename,&test)==-1) return -1;
    //get a unused file discriptor
    uint32_t unusedfd = FILE_TYPE_2;
    for(unusedfd = FILE_TYPE_2;unusedfd<=MAX_FILES;unusedfd++)
    {
       if(unusedfd==MAX_FILES) return -1;
       if(pcb_array[current_pid].fd_array[unusedfd].flags == NOT_IN_USE_FLAG)
            break;
    }

    switch(test.filetype)
    {
        case 0://rtc
        {

            pcb_array[current_pid].fd_array[unusedfd].fops = (uint32_t)rtc_jumptable;
            pcb_array[current_pid].fd_array[unusedfd].inode = 0;
            pcb_array[current_pid].fd_array[unusedfd].fp = 0;
            pcb_array[current_pid].fd_array[unusedfd].flags = IN_USE_FLAG;
            break;
        }
        case 1://directory
        {


            pcb_array[current_pid].fd_array[unusedfd].fops = (uint32_t)directory_jumptable;
            pcb_array[current_pid].fd_array[unusedfd].inode = 0;
            pcb_array[current_pid].fd_array[unusedfd].fp = 0;
            pcb_array[current_pid].fd_array[unusedfd].flags = IN_USE_FLAG;
            break;
        }
        case FILE_TYPE_2://file
        {


            pcb_array[current_pid].fd_array[unusedfd].fops = (uint32_t)file_jumptable;
            pcb_array[current_pid].fd_array[unusedfd].inode = test.inode_num;
            pcb_array[current_pid].fd_array[unusedfd].fp = 0;
            pcb_array[current_pid].fd_array[unusedfd].flags = IN_USE_FLAG;
            break;
        }
        default:
            return -1;


    }
 
    uint32_t* ptr = (uint32_t*)pcb_array[current_pid].fd_array[unusedfd].fops; 
    int32_t (*fun_ptr)(const uint8_t*) = (void*)ptr[0];
    (*fun_ptr)(filename);

    
    return unusedfd;
}

int32_t close (int32_t fd)
{
	if(fd > MAX_FILES-1 || fd < FILE_TYPE_2){
		return -1;
	}
	pcb_array[current_pid].fd_array[fd].flags = NOT_IN_USE_FLAG;
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
