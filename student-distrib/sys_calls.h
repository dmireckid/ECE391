/* sys_calls.h - system calls
 */

#ifndef _SYS_CALLS_H
#define _SYS_CALLS_H
#include "types.h"

#define IN_USE_FLAG 33
#define NOT_IN_USE_FLAG -44

typedef struct __attribute__ ((packed))  file_entry{

    uint32_t fops	: 32; //file operations table pointer	
    uint32_t inode	: 32; //inode
    uint32_t fp		: 32;//file position
    uint32_t flags	: 32; //flags
		 
    
}file_entry_t;

#define MAX_FILES 8
typedef struct __attribute__ ((packed)) pcb{

    file_entry_t fd_array[MAX_FILES] ;	
    uint32_t parent_pid;
    uint32_t parent_kernel_esp;
    uint32_t parent_kernel_ebp;
    uint32_t files_in_use;
		
}pcb_t;





void init_STD(uint32_t pid);


int32_t halt(uint8_t status);
int32_t execute(const uint8_t* command);
int32_t read (int32_t fd, void* buf, int32_t nbytes);
int32_t write (int32_t fd, const void* buf, int32_t nbytes);
int32_t open (const uint8_t* filename);
int32_t close (int32_t fd);
int32_t getargs (uint8_t* buf, int32_t nbytes);
int32_t vidmap (uint8_t** screen_start);
int32_t set_handler (int32_t signum, void* handler_address);
int32_t sigreturn (void);

#endif
