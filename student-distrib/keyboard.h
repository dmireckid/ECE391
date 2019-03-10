/* keyboard.h - Defines used in interactions with keyboard
 * vim:ts=4 noexpandtab
 */

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

/* Keyboard IRQ number and ports to communicate*/
#define KEYBOARD_IRQ    1
#define KEYBOARD_DATA   0x60
#define KEYBOARD_STATUS 0x64

/* Initializes keyboard */
void keyboard_init(void);

/* Handles input from keyboard and prints it on screen */
void keyboard_handler_function(void);

#endif /* KEYBOARD_H */
