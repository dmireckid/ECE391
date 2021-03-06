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
#include "term_switch.h"
#include "types.h"
#include "pit.h"

static uint32_t rtc_jumptable[ELF_SIZE] = { (uint32_t)&rtc_open,(uint32_t)&rtc_read,(uint32_t)&rtc_write,(uint32_t)&rtc_close};
static uint32_t terminal_jumptable[ELF_SIZE] = {(uint32_t)&terminal_open,(uint32_t)&terminal_read,(uint32_t)&terminal_write,(uint32_t)&terminal_close};
static uint32_t directory_jumptable[ELF_SIZE] = {(uint32_t)&open_d,(uint32_t)&read_d,(uint32_t)&write_d,(uint32_t)&close_d};
static uint32_t file_jumptable[ELF_SIZE] = {(uint32_t)&open_f,(uint32_t)&read_f,(uint32_t)&write_f,(uint32_t)&close_f};

static int8_t elf_string[ELF_SIZE] = {ELF_0,ELF_1,ELF_2,ELF_3};


static uint32_t num_processes=0;
//index 0 is skipped for our implementation, 1-6 will hold the base shell and command programs
static pcb_t pcb_array[MAX_PROCESSES+1];


/*
 *	halt 
 *
 *	INPUTS: uint8_t status - the number corresponding to how halt was called
 *	OUTPUTS: none
 *	RETURN VALUE: technically returns for execute
 *	SIDE EFFECTS: program is removed from PCB array
 */
int32_t halt (uint8_t status){

	int j;
	for (j = 0; j<LINE_BUFFER_SIZE; j++) {
		terminal_array[PIT_terminal].keyboard[j] = '\0';
	}
	terminal_array[PIT_terminal].buf_count = 0;

    if(terminal_array[PIT_terminal].curr_pid==1 ||terminal_array[PIT_terminal].curr_pid==2|| terminal_array[PIT_terminal].curr_pid==3)
    {
        num_processes--;
        pcb_array[terminal_array[PIT_terminal].curr_pid].in_use_flag = NOT_IN_USE_FLAG;
        //terminal_array[PIT_terminal].curr_pid = pcb_array[terminal_array[PIT_terminal].curr_pid].parent_pid;
        uint32_t kernel_stack_bottom = MB_8 - terminal_array[PIT_terminal].curr_pid*KB_8;
        if(terminal_array[PIT_terminal].curr_pid != PIT_terminal)
        {
            while(1)
            {
                printf("\nhalt error 1\n");
            }
        }
        asm volatile(
		"movl %0,%%esp;"
		"movl %1, %%ebp;"
        : 
        : "r"(kernel_stack_bottom), "r"(kernel_stack_bottom)
        : "memory");

        execute((const uint8_t*)"shell");

    }
	
	// clear out the fd_array associated with the program being halted
	int i;
	for (i = FILE_TYPE_2; i < MAX_FILES; i++) {
		close(i);
	}

	// write parent process back info to tss.esp0
	//tss.esp0 = pcb_array[current_pid].parent_kernel_esp;
	
	// set the in_use_flag of the pcb we're going to leave to NOT_IN_USE
	pcb_array[terminal_array[PIT_terminal].curr_pid].in_use_flag = NOT_IN_USE_FLAG;
	
	// get new pid after halt is done
	uint32_t old_pid = terminal_array[PIT_terminal].curr_pid;
	uint32_t new_pid = pcb_array[terminal_array[PIT_terminal].curr_pid].parent_pid;
	terminal_array[PIT_terminal].curr_pid = new_pid;

	// decrement number of processes
	num_processes--;
	
	//set tss.esp0 back to original address
    tss.esp0 = MB_8 - new_pid*KB_8;
    //printf("\nhalt1: %x %x %x\n",tss.esp0,current_pid,pcb_array[current_pid].parent_kernel_esp);
	
	// restore paging
	remap_page(new_pid);

	// set the return value for execute
	uint32_t ret_val;
	if (status == 0)
		ret_val = 0;
	else if (status == EX_STATUS)
		ret_val = EXCEPTION;
	else
		ret_val = ABNORMAL;
    
    //printf("\nhalt2: %x %x %x\n",tss.esp0,current_pid,pcb_array[current_pid].parent_kernel_esp);
	// move parent esp and ebp values back into esp/ebp registers
    asm volatile(
		"movl %0, %%eax;"
		"movl %1,%%esp;"
		"movl %2, %%ebp;"
		"jmp after_iret"
        : 
        : "r"(ret_val),"r"(pcb_array[old_pid].parent_kernel_esp), "r"(pcb_array[old_pid].parent_kernel_ebp)
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
	//check if command pointer is NULL
	if(command == NULL) return -1;


	//check if the max number of processes are running
	if (num_processes == MAX_PROCESSES) {
		printf("Max number of processes reached \n");
		return 0;
	}

	// parse the execute command from the original buffer
    // '\0', ' ', '\n'
    uint8_t exe[LINE_BUFFER_SIZE];exe[0] = '\0';
    int i=0; 
	while (i<LINE_BUFFER_SIZE && command[i] == ' ') {
		i++;
	}
	int k=0;
    while(i<LINE_BUFFER_SIZE && command[i]!='\0' && command[i]!=' '&& command[i]!= '\n')
    {
        exe[k] = command[i];
        k++;
		i++;
    }
    exe[k]='\0';
	if (exe[0] == '\n' || exe[0]=='\0') return 0;

	// move through the rest of the spaces that occur after the command
	while(i<LINE_BUFFER_SIZE && command[i]==' ')
	{
		i++;
	}

    dentry_t test;
    int8_t buf[ELF_SIZE];
    int32_t file_size;
    //check if file exists
    if(read_dentry_by_name(exe,&test)==-1) return -1;
	//check if the filetype is a file
    if(test.filetype != FILE_TYPE_2) return AB_STATUS;
    //check if the first 4 bytes are ELF magic number
    file_size = read_data(test.inode_num,0,(uint8_t*) buf,ELF_SIZE);
    if(strncmp(buf,elf_string,ELF_SIZE)!=0) return AB_STATUS;


	//assign pid
	uint32_t new_pid = 1;
	while (pcb_array[new_pid].in_use_flag != NOT_IN_USE_FLAG) {
		new_pid++;
	}
	pcb_array[new_pid].in_use_flag = IN_USE_FLAG;

	//increment number of processes
	num_processes++;
  
	//uint8_t* te = (uint8_t*)0x08048000;
	//uint32_t blah = *te;

    int j=0;
    //save args for getargs 
    if (command[i]=='\0' || command[i]=='\n') 
    {
        pcb_array[new_pid].args[j] ='\0';
    }
    else
    {
        while(i<LINE_BUFFER_SIZE && command[i]!='\0' && command[i]!= '\n')
        {
            pcb_array[new_pid].args[j] =command[i];
            i++;j++;
        }
        pcb_array[new_pid].args[j] ='\0';
    }


    //assign memory for the process
    remap_page(new_pid);

    //copy program into memory
    read_data(test.inode_num,0,(uint8_t*)PROGRAM_VIRTUAL_ADDRESS,PROGRAM_SIZE);

    //Initialize PCB values and stdin/out file descriptors
    init_STD(new_pid);


    if( new_pid >= 1 && new_pid <= 3 )
    {
        pcb_array[new_pid].parent_pid = 0;
    }
    else
    {
        pcb_array[new_pid].parent_pid = terminal_array[PIT_terminal].curr_pid;
    }
    

	//save parent esp and ebp values
    asm volatile("movl %%esp,%%eax;"
        : "=a"(pcb_array[new_pid].parent_kernel_esp)
        :
        : "memory");
    asm volatile("movl %%ebp,%%eax;" 
    : "=a"(pcb_array[new_pid].parent_kernel_ebp)
    :
    : "memory");

	//set tss values
    tss.esp0 = MB_8 - new_pid*KB_8;
	tss.ss0 = KERNEL_DS;
    //printf("\nexecute1: %x %x %x\n",tss.esp0,new_pid,pcb_array[new_pid].parent_kernel_esp);

    //get entry point
	uint32_t entry;
	read_data(test.inode_num,INDEX_24,(uint8_t*)&entry,ELF_SIZE);

	uint32_t user_ds = USER_DS; //store USER_DS in a variable
	uint32_t user_cs = USER_CS; //store USER_CS in a variable
	uint32_t iret_esp = PROGRAM_VIRTUAL_END; //store the IRET esp in a variable

	//update the current terminal pid
	terminal_array[PIT_terminal].curr_pid = new_pid;

	//next is context switching, so close interrupts
	cli();
    //printf("\nexecute2: %x %x %x\n",tss.esp0,new_pid,pcb_array[current_pid].parent_kernel_esp);
	context_switch(user_ds, iret_esp, user_cs, entry);

    return 0;
}

/*
 *	init_pcb_array
 *
 *	INPUTS: none
 *	OUTPUTS: none
 *	RETURN VALUE: none
 *	SIDE EFFECTS: intializes all pcb's flags to unused
 */
void init_pcb_array()
{
	int i;
	for (i = 0; i < MAX_PROCESSES+1; i++) {
		pcb_array[i].in_use_flag = NOT_IN_USE_FLAG;
	}
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
	// set stdin
    pcb_array[pid].fd_array[0].fops = (uint32_t)terminal_jumptable;
    pcb_array[pid].fd_array[0].inode =0;
    pcb_array[pid].fd_array[0].fp =0;
    pcb_array[pid].fd_array[0].flags =IN_USE_FLAG;

	// set stdout
    pcb_array[pid].fd_array[1].fops = (uint32_t)terminal_jumptable;
    pcb_array[pid].fd_array[1].inode =0;
    pcb_array[pid].fd_array[1].fp =0;
    pcb_array[pid].fd_array[1].flags =IN_USE_FLAG;

	// set all other file descriptors as not in use
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
 *	RETURN VALUE: number of bytes read, or -1 on failure
 *	SIDE EFFECTS: runs the read command based on the file type
 */
int32_t read (int32_t fd, void* buf, int32_t nbytes)
{
    //check if file descriptor is in bounds and if the flag is IN_USE
    if(fd > MAX_FILES-1 || fd < 0) return -1;
	if(pcb_array[terminal_array[PIT_terminal].curr_pid].fd_array[fd].flags == NOT_IN_USE_FLAG) return -1;
	
	//if the fd called is stdout, return -1
	if(fd==1) return -1;
 
	//jump to the corresponding read function
    uint32_t* ptr = (uint32_t*)pcb_array[terminal_array[PIT_terminal].curr_pid].fd_array[fd].fops; 
    int32_t (*fun_ptr)(int32_t, void*, int32_t) = (void*)ptr[1];
    return (*fun_ptr)(fd,buf,nbytes);
}


/*
 *	get_flags
 *
 *	INPUTS: int32_t fd - file descriptor
 *	OUTPUTS: none
 *	RETURN VALUE: flags of specified fd
 *	SIDE EFFECTS: none
 */
uint32_t get_flags(int32_t fd){
	return pcb_array[terminal_array[PIT_terminal].curr_pid].fd_array[fd].flags;
}
/*
 *	get_inode
 *
 *	INPUTS: int32_t fd - file descriptor
 *	OUTPUTS: none
 *	RETURN VALUE: value of inode of specified fd
 *	SIDE EFFECTS: none
 */
uint32_t get_inode(int32_t fd){
	return pcb_array[terminal_array[PIT_terminal].curr_pid].fd_array[fd].inode;
}
/*
 *	get_fp
 *
 *	INPUTS: int32_t fd - file descriptor
 *	OUTPUTS: none
 *	RETURN VALUE: value of fp
 *	SIDE EFFECTS: none
 */
uint32_t get_fp(int32_t fd){
	return pcb_array[terminal_array[PIT_terminal].curr_pid].fd_array[fd].fp;
}
/*
 *	set_fp
 *
 *	INPUTS: int32_t fd - file descriptor
 *	OUTPUTS: none
 *	RETURN VALUE: 
 *	SIDE EFFECTS: changes fp of specified fd
 */
void set_fp(int32_t fd,uint32_t fp){
	pcb_array[terminal_array[PIT_terminal].curr_pid].fd_array[fd].fp = fp;
}
/*
 *	clear_fp
 *
 *	INPUTS: int32_t fd - file descriptor
 *	OUTPUTS: none
 *	RETURN VALUE: none
 *	SIDE EFFECTS: clears fp of specified fd
 */
void clear_fp(int32_t fd){
	pcb_array[terminal_array[PIT_terminal].curr_pid].fd_array[fd].fp = 0;
	return;
}
/*
 *	fp_plus
 *
 *	INPUTS: int32_t fd - file descriptor
 *	OUTPUTS: none
 *	RETURN VALUE: none
 *	SIDE EFFECTS: increment fp of specified fd
 */
void fp_plus(int32_t fd){
	pcb_array[terminal_array[PIT_terminal].curr_pid].fd_array[fd].fp++;
	return;
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
	if(pcb_array[terminal_array[PIT_terminal].curr_pid].fd_array[fd].flags == NOT_IN_USE_FLAG) return -1;
	
	//if the fd called is stin, return -1
	if(fd==0) return -1;
 
	//jump to the corresponding write function
    uint32_t* ptr = (uint32_t*)pcb_array[terminal_array[PIT_terminal].curr_pid].fd_array[fd].fops; 
    int32_t (*fun_ptr)(int32_t, const void*, int32_t) = (void*)ptr[FILE_TYPE_2];
    return (*fun_ptr)(fd,buf,nbytes);
}

/*
 *	open
 *
 *	INPUTS: const uint8_t* filename - pointer to file name
 *	OUTPUTS: none
 *	RETURN VALUE: file descriptor value
 *				  If the named file does not exist or no descriptors are free, the call returns -1.
 *	SIDE EFFECTS: runs the open command based on the file type
 */
int32_t open (const uint8_t* filename)
{
    dentry_t test;
    //check if file exists
    if(read_dentry_by_name(filename,&test)==-1) return -1;
    //get an unused file descriptor
    uint32_t unusedfd = FILE_TYPE_2;
    for(unusedfd = FILE_TYPE_2; unusedfd<=MAX_FILES; unusedfd++)
    {
       if(unusedfd==MAX_FILES) return -1;	//if all file descriptors are in use, return -1
       if(pcb_array[terminal_array[PIT_terminal].curr_pid].fd_array[unusedfd].flags == NOT_IN_USE_FLAG)
            break;
    }

	//set the file descriptor values based on file type
    switch(test.filetype)
    {
        case 0://rtc
        {

            pcb_array[terminal_array[PIT_terminal].curr_pid].fd_array[unusedfd].fops = (uint32_t)rtc_jumptable;
            pcb_array[terminal_array[PIT_terminal].curr_pid].fd_array[unusedfd].inode = 0;
            pcb_array[terminal_array[PIT_terminal].curr_pid].fd_array[unusedfd].fp = 0;
            pcb_array[terminal_array[PIT_terminal].curr_pid].fd_array[unusedfd].flags = IN_USE_FLAG;
            break;
        }
        case 1://directory
        {


            pcb_array[terminal_array[PIT_terminal].curr_pid].fd_array[unusedfd].fops = (uint32_t)directory_jumptable;
            pcb_array[terminal_array[PIT_terminal].curr_pid].fd_array[unusedfd].inode = 0;
            pcb_array[terminal_array[PIT_terminal].curr_pid].fd_array[unusedfd].fp = 0;
            pcb_array[terminal_array[PIT_terminal].curr_pid].fd_array[unusedfd].flags = IN_USE_FLAG;
            break;
        }
        case FILE_TYPE_2://file
        {


            pcb_array[terminal_array[PIT_terminal].curr_pid].fd_array[unusedfd].fops = (uint32_t)file_jumptable;
            pcb_array[terminal_array[PIT_terminal].curr_pid].fd_array[unusedfd].inode = test.inode_num;
            pcb_array[terminal_array[PIT_terminal].curr_pid].fd_array[unusedfd].fp = 0;
            pcb_array[terminal_array[PIT_terminal].curr_pid].fd_array[unusedfd].flags = IN_USE_FLAG;
            break;
        }
        default:
            return -1;


    }
 
	//jump to the corresponding open function
    uint32_t* ptr = (uint32_t*)pcb_array[terminal_array[PIT_terminal].curr_pid].fd_array[unusedfd].fops; 
    int32_t (*fun_ptr)(const uint8_t*) = (void*)ptr[0];
    (*fun_ptr)(filename);

    return unusedfd;
}

/*
 *	close
 *
 *	INPUTS: int32_t fd - file descriptor value
 *	OUTPUTS: none
 *	RETURN VALUE: 0 if the close was successful, -1 if fd is out of bounds
 *	SIDE EFFECTS: makes the certain file descriptor flagged as not in use
 */
int32_t close (int32_t fd)
{
	// check if fd is within range and set it to not in use, return -1 if fd is out of bounds
	if(fd > MAX_FILES-1 || fd < FILE_TYPE_2){
		return -1;
	}
	// check if fd is unopened, if so, return -1
	if (pcb_array[terminal_array[PIT_terminal].curr_pid].fd_array[fd].flags == NOT_IN_USE_FLAG) {
		return -1;
	}
	
	// set the flag of the now-closed fd to NOT_IN_USE
	pcb_array[terminal_array[PIT_terminal].curr_pid].fd_array[fd].flags = NOT_IN_USE_FLAG;
    return 0;
}

/*
 *	getargs
 *
 *	INPUTS: uint8_t* buf - pointer to a buffer
 *			int32_t nbytes - size of the buffer
 *	OUTPUTS: none
 *	RETURN VALUE: Return 0 for success
 *   If there are no arguments, 
 *   or if the arguments and a terminal NULL (0-byte) do not fit in the buffer, simply return -1
 *  
 *	SIDE EFFECTS: modifies buffer / userspace memory
 */
int32_t getargs (uint8_t* buf, int32_t nbytes)
{
	// check if buffer pointer is NULL or if the args buffer in the pcb is empty
	if (buf == NULL) return -1;
	if(nbytes < LINE_BUFFER_SIZE || pcb_array[terminal_array[PIT_terminal].curr_pid].args[0] =='\0' ) return -1;

	// insert the args buffer into the argument buffer
    int32_t i = 0;
    while (pcb_array[terminal_array[PIT_terminal].curr_pid].args[i]!= '\0' && i<LINE_BUFFER_SIZE) {
        buf[i] = pcb_array[terminal_array[PIT_terminal].curr_pid].args[i];
        i++;
    }
    buf[i] = '\0';
    return 0;

}

/*
 *	vidmap
 *
 *	INPUTS: uint8_t** screen_start - pointer to virtual address
 *	OUTPUTS: none
 *	RETURN VALUE: 0 for success, -1 for failure
 *	SIDE EFFECTS: maps a user-accessible virtual address to the physical video memory
 */
int32_t vidmap (uint8_t** screen_start)
{
	// check if pointer is NULL, if so, return -1
	if (screen_start == NULL) return -1;
	// check if pointer is an address within a user-level page, if so, return -1
	if (screen_start < (uint8_t**)PROGRAM_VIRTUAL_ADDRESS || screen_start > (uint8_t**)PROGRAM_VIRTUAL_END) return -1;
	
	// map the video memory to a virtual address that permits access to user
	vid_page();
	// if the terminal that will vidmap is not currently being displayed, remap vidmap so that it points to the buffer instead
	if (curr_term_num != PIT_terminal) {
		remap_shadow(PIT_terminal);
	}
	*screen_start = (uint8_t*)VID_MAP_ADDR;
    return 0;
}

/*
 *	set_handler
 *
 *	INPUTS: int32_t signum -
 *			void* handler_address -
 *	OUTPUTS: none
 *	RETURN VALUE:
 *	SIDE EFFECTS:
 */
int32_t set_handler (int32_t signum, void* handler_address)
{
    return -1;
}

/*
 *	sigreturn
 *
 *	INPUTS: none
 *	OUTPUTS: none
 *	RETURN VALUE:
 *	SIDE EFFECTS:
 */
int32_t sigreturn (void)
{
    return -1;
}
