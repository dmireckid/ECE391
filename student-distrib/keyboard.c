/* keyboard.c - Functions to initialize and handle keyboard interrupts
 * vim:ts=4 noexpandtab
 */

#include "keyboard.h"
#include "lib.h"
#include "i8259.h"

/* maps keycode to ASCII character code */
char keymap[256] =  {   '\0', '\0' /*0x01: escape*/,							/* 0x00: not used, 0x01: esc key */
                        '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',			/* 0x02~0x0B: numbers 1~9 and 0 */ 
						'-', '=', '\0' /*0x0E: backspace*/, '\0' /*0x0F: tab*/,		/* 0x0C~0x0F */
						'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',			/* 0x10~0x19: letters q~p */
						'[', ']', '\n' /*0x1C: enter*/, '\0' /*0x1D: left ctrl*/,	/* 0x1A~0x1D */
						'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',				/* 0x1E~0x26: letters a~l */
						';', '\'', '`', '\0' /*0x2A: left shift*/, '\\',			/* 0x27~0x2B */
						'z', 'x', 'c', 'v', 'b', 'n', 'm',							/* 0x2C~0x32: letters z~m */
						',', '.', '/'												/* 0x33~0x35 */
                    };

/* flags for Left Shift, Right Shift, Left Control, Right Control, and Caps Lock */
uint8_t lctrl = 0;
uint8_t rctrl = 0;
uint8_t shift_pressed = 0;
uint8_t caps_lock = 0;

/* 
 * keyboard_init()
 *   Description: Initializes keyboard
 *         Input: None
 *        Output: None
 *        Return: None
 *  Side Effects: Initializes keyboard by enabling the KEYBOARD_IRQ
 */
void keyboard_init() {
    enable_irq(KEYBOARD_IRQ);
}

/* 
 * keyboard_handler_function()
 *   Description: Handles an inputted keycode from keyboard and prints the corresponding character on screen 
 *         Input: None
 *        Output: None
 *        Return: None
 *  Side Effects: Sends EOI to KEYBOARD_IRQ and prints an input on the screen
 */
void keyboard_handler_function() {
    /* signal EOI to allow further IRQs */
    send_eoi(KEYBOARD_IRQ);

    /* contains keyboard status and keycode */
    uint8_t status, keycode;

    /* get status number */
    status = (uint8_t)(inb(KEYBOARD_STATUS));

    /* if status is zero, there is no data to read */
    if (status & 0x01) {
        /* get keycode */
        keycode = (uint8_t)(inb(KEYBOARD_DATA));

		/* if the keycode received is anything above F1 being pressed, don't do anything unless it's a release from a Shift key or Control key */
		if ( (uint8_t)keycode >= F1_P ) {
			if ( (uint8_t)keycode == LSHIFT_R || (uint8_t)keycode == RSHIFT_R ) {
				shift_pressed = 0;
			}
			if ( (uint8_t)keycode == LCTRL_R ) {
				lctrl = 0;
			}
			return;
		}
		
		/* if the keycode received is Left Alt, don't do anything */
		if ( (uint8_t)keycode == LALT )
			return;

		/* if the key that's pressed is Left Control, set its flag to 1 */
		if ((uint8_t)keycode == LCTRL_P) {
			lctrl = 1;
			return;
		}
		
		/* if the key that's pressed is Backspace, clear the previously entered symbol and move the cursor back */
		if ((uint8_t)keycode == BACKSPACE) {
			backspace();
			return;
		}
		
		/* if the key that's pressed is Caps Lock, toggle the caps_lock flag */
		if ((uint8_t)keycode == CAPSLOCK) {
			if ( caps_lock == 0 )
				caps_lock = 1;
			else
				caps_lock = 0;
			return;
		}
		
		/* if the key that's pressed is Left Shift or Right Shift, toggle their respective flags */
		if ((uint8_t)keycode == LSHIFT_P || (uint8_t)keycode == RSHIFT_P) {
			shift_pressed = 1;
			return;
		}

        /* prints the pressed key on screen while checking if Caps Lock has been toggled and/or if Shift is being pressed */
        if ( (caps_lock^shift_pressed) == 1 ) {
			if ( (((uint8_t)keycode >= Q_MAP) && ((uint8_t)keycode <= P_MAP)) || (((uint8_t)keycode >= A_MAP) && ((uint8_t)keycode <= L_MAP)) || (((uint8_t)keycode >= Z_MAP) && ((uint8_t)keycode <= M_MAP)) ) {
				putc(keymap[(uint8_t)keycode]-CAP_OFFSET);
				return;
			}
		}
		if ( lctrl == 1 && keymap[(uint8_t)keycode] == 'l' ) {
			ctrl_l();
			return;
		}
		putc(keymap[(uint8_t)keycode]);
    }
}
