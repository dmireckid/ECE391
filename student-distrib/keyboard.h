/* keyboard.h - Defines used in interactions with keyboard
 * vim:ts=4 noexpandtab
 */

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

/* Keyboard IRQ number and ports to communicate*/
#define KEYBOARD_IRQ    1
#define KEYBOARD_DATA   0x60
#define KEYBOARD_STATUS 0x64

/* Number of available ASCII characters */
#define NUM_ASCII		256

/* Pre-defined keycodes for Left Shift, Right Shift, Backspace, Caps Lock, Left Alt, Control, and F1 */
#define F1_P			0x3B
#define LALT			0x38
#define CTRL_P			0x1D
#define CTRL_R			0x9D
#define BACKSPACE		0x0E
#define CAPSLOCK		0x3A
#define LSHIFT_P		0x2A
#define LSHIFT_R		0xAA
#define RSHIFT_P		0x36
#define RSHIFT_R		0xB6

/* Keycodes for symbols */


/* Offset between lower and upper case letters and mapped locations of letters in keycode table */
#define CAP_OFFSET		32
#define Q_MAP			0x10
#define P_MAP			0x19
#define A_MAP			0x1E
#define L_MAP			0x26
#define Z_MAP			0x2C
#define M_MAP			0x32

/* Initializes keyboard */
void keyboard_init(void);

/* Initializes shift-symbol map */
void shift_init(void);

/* Handles input from keyboard and prints it on screen */
void keyboard_handler_function(void);

#endif /* KEYBOARD_H */
