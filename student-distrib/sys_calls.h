/* sys_calls.h - system calls
 */

#ifndef _SYS_CALLS_H
#define _SYS_CALLS_H
#include "types.h"

#define IN_USE_FLAG 			33
#define NOT_IN_USE_FLAG 		44

#define MAX_PROCESSES 			6
#define MAX_FILES 				8
#define FILE_TYPE_2				2

#define ELF_SIZE 				4
#define ELF_0					0x7f
#define ELF_1					0x45
#define ELF_2					0x4c
#define ELF_3					0x46

#define INDEX_24				24
#define PID_SHELL_0  			0
#define PID_PROGRAM_0  			1
#define PROGRAM_VIRTUAL_ADDRESS 0x08048000
#define PROGRAM_VIRTUAL_END		0x83FFFFC

#define	EX_STATUS				8
#define EXCEPTION				256
#define ABNORMAL				125
#define AB_STATUS				3


typedef struct __attribute__((packed))  file_entry{

    uint32_t fops	: 32; //file operations table pointer	
    uint32_t inode	: 32; //inode
    uint32_t fp		: 32; //file position
    uint32_t flags	: 32; //flags

}file_entry_t;

typedef struct __attribute__((packed)) pcb{

    file_entry_t fd_array[MAX_FILES] ;	
    uint32_t parent_pid;
    uint32_t parent_kernel_esp;
    uint32_t parent_kernel_ebp;
    uint8_t args[128];
	uint8_t flag;
    
}pcb_t;

void set_fp(int32_t fd,uint32_t fp);
uint32_t get_flags(int32_t fd);
uint32_t get_inode(int32_t fd);
void init_pcb_array();
void init_STD(uint32_t pid);
uint32_t get_fp(int32_t fd);
void clear_fp(int32_t fd);
void fp_plus(int32_t fd);

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
