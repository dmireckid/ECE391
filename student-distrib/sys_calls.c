/* sys_calls.c - system calls
 */

#include "sys_calls.h"




int32_t halt (uint8_t status){
    return 0;

}

int32_t execute (const uint8_t* command)
{
    return 0;
}


int32_t read (int32_t fd, void* buf, int32_t nbytes){}
int32_t write (int32_t fd, const void* buf, int32_t nbytes){}
int32_t open (const uint8 t* filename){}
int32_t close (int32_t fd){}
int32_t getargs (uint8 t* buf, int32_t nbytes){}
int32_t vidmap (uint8 t** screen start){}
int32_t set_handler (int32_t signum, void* handler_address){}
int32_t sigreturn (void){}


