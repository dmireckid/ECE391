/* sys_calls.h - system calls
 */

#ifndef _SYS_CALLS_H
#define _SYS_CALLS_H
#include "types.h"

typedef struct __attribute__ ((packed))  file_entry{

    uint32_t fops	: 32; //file operations table pointer	
    uint32_t inode	: 32; //inode
    uint32_t fp		: 32;//file position
    uint32_t flags	: 32; //flags
		 
    
}file_entry_t;




int32_t halt(uint8_t status);
int32_t execute(const uint8_t* command);
int32_t read (int32_t fd, void* buf, int32_t nbytes);
int32_t write (int32_t fd, const void* buf, int32_t nbytes);
int32_t open (const uint8 t* filename);
int32_t close (int32_t fd);
int32_t getargs (uint8 t* buf, int32_t nbytes);
int32_t vidmap (uint8 t** screen start);
int32_t set_handler (int32_t signum, void* handler_address);
int32_t sigreturn (void);
