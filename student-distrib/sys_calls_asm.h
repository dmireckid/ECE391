/* sys_calls_asm.h - assembly linkage for system calls
 */



#ifndef _SYS_CALLS_ASM_H
#define _SYS_CALLS_ASM_H

#include "sys_calls.h"

extern void sys_call_handler();

extern void context_switch(uint32_t user_ds, uint32_t iret_esp, uint32_t user_cs, uint32_t entry);

#endif
