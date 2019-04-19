#ifndef _PIT_ASM
#define _PIT_ASM

#include "pit.h"

/* keyboard_handler function from keyboard_asm.S file called externally by interrupt handler */
extern void pit_handler(void);

#endif
