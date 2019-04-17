/* keyboard.h - Defines used in interactions with keyboard
 * vim:ts=4 noexpandtab
 */

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "types.h"

/* Keyboard IRQ number and ports to communicate*/
#define KEYBOARD_IRQ    1
#define KEYBOARD_DATA   0x60
#define KEYBOARD_STATUS 0x64

/* Number of available ASCII characters */
#define NUM_ASCII		256

/* Pre-defined keycodes for Left Shift, Right Shift, Backspace, Caps Lock, Control */
#define CTRL_P			0x1D
#define CTRL_R			0x9D
#define BACKSPACE		0x0E
#define CAPSLOCK		0x3A
#define LSHIFT_P		0x2A
#define LSHIFT_R		0xAA
#define RSHIFT_P		0x36
#define RSHIFT_R		0xB6

/* Pre-defined keycodes for Alt, F1, F2, and F3 */
#define ALT_P			0x38
#define ALT_R			0xB8
#define F1_P			0x3B
#define F2_P			0x3C
#define F3_P			0x3D

/* Offset between lower and upper case letters and mapped locations of letters in keycode table */
#define CAP_OFFSET		32
#define Q_MAP			0x10
#define P_MAP			0x19
#define A_MAP			0x1E
#define L_MAP			0x26
#define Z_MAP			0x2C
#define M_MAP			0x32

/* Keycode values for shift key symbol mapping */
#define KB_ONE                  0x02
#define KB_TWO	                0x03
#define KB_THREE	            0x04
#define KB_FOUR                 0x05
#define KB_FIVE                 0x06
#define KB_SIX                  0x07
#define KB_SEVEN                0x08
#define KB_EIGHT                0x09
#define KB_NINE                 0x0A
#define KB_ZERO                 0x0B
#define KB_BACKTICK             0x29
#define KB_DASH                 0x0C
#define KB_EQUAL                0x0D
#define KB_OPENING_SQ_BRACKET   0x1A
#define KB_CLOSING_SQ_BRACKET   0x1B
#define KB_BACKSLASH            0x2B
#define KB_SEMICOLON            0x27
#define KB_APOSTROPHE           0x28
#define KB_COMMA                0x33
#define KB_DOT                  0x34
#define KB_SLASH                0x35

/* Initializes keyboard */
void keyboard_init(void);

/* Initializes shift-symbol map */
void shift_init(void);

/* Handles input from keyboard and prints it on screen */
void keyboard_handler_function(void);

int32_t keyboard_open(const uint8_t* filename);

int32_t keyboard_close(int32_t fd);

int32_t keyboard_write(int32_t fd, const void* buf, int32_t nbytes);

#endif /* KEYBOARD_H */
