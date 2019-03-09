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

/* maps keycode to character code */
uint8_t keymap[128] = { /* ... */ 'a', 'b' /* ... */ };

void keyboard_init(void);

void keyboard_handler_function(void);

#endif /* KEYBOARD_H */
