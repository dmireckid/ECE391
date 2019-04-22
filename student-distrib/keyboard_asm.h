/* keyboard_asm.h - Defines used in communication between interrupt and keyboard handlers
 * vim:ts=4 noexpandtab
 */

#ifndef _KEYBOARD_ASM
#define _KEYBOARD_ASM

#include "keyboard.h"

/* keyboard_handler function from keyboard_asm.S file called externally by interrupt handler */
extern void keyboard_handler(void);
extern uint32_t test_cr2(void);


#endif
