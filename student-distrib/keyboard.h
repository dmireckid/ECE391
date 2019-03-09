/* keyboard.h - Defines used in interactions with keyboard
 * vim:ts=4 noexpandtab
 */

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "lib.h"
#include "i8259.h"

/* Keyboard IRQ number and ports to communicate*/
#define KEYBOARD_IRQ    1
#define KEYBOARD_DATA   0x60
#define KEYBOARD_STATUS 0x64

/* maps keycode to ASCII character code */
char keymap[256] =  {   '\0', '27', /* 0x00: not used, 0x01: esc key */
                        '49', '50', '51', '52', '53', '54', '55', '56', '57', '48' /* 0x02~0x0B: numbers 1~9 and 0 */ 
                    };

void keyboard_init(void);

void keyboard_handler_function(void);

#endif /* KEYBOARD_H */
