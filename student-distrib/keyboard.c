/* keyboard.c - Functions to initialize and handle keyboard interrupts
 * vim:ts=4 noexpandtab
 */

#include "keyboard.h"
#include "lib.h"
#include "i8259.h"
#include "term_driver.h"

/* maps keycode to ASCII character code */
char keymap[NUM_ASCII] =  {   '\0', '\e' /*0x01: escape*/,							/* 0x00: not used, 0x01: esc key */
                        '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',			/* 0x02~0x0B: numbers 1~9 and 0 */ 
						'-', '=', '\b' /*0x0E: backspace*/, '\t' /*0x0F: tab*/,		/* 0x0C~0x0F */
						'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',			/* 0x10~0x19: letters q~p */
						'[', ']', '\n' /*0x1C: enter*/, '\0' /*0x1D: left ctrl*/,	/* 0x1A~0x1D */
						'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',				/* 0x1E~0x26: letters a~l */
						';', '\'', '`', '\0' /*0x2A: left shift*/, '\\',			/* 0x27~0x2B */
						'z', 'x', 'c', 'v', 'b', 'n', 'm',							/* 0x2C~0x32: letters z~m */
						',', '.', '/', '\0', '\0', '\0', ' '						/* 0x33~0x38: other keys, 0x39: space key */
                    };

/* First column contains keycodes for symbol keys */
/* and the second column contains ascii codes for symbols when shifted */
char shifted_symbols_map[21][2] = {	/* symbols on number keys */
									{0x02, '!'},	// 1 to !
									{0x03, '@'},	// 2 to @
									{0x04, '#'},	// 3 to #
									{0x05, '$'},    // 4 to $
									{0x06, '%'},    // 5 to %
									{0x07, '^'},    // 6 to ^
									{0x08, '&'},    // 7 to &
									{0x09, '*'},    // 8 to *
									{0x0A, '('},    // 9 to (
									{0x0B, ')'},    // 0 to )

									/* other symbols */
									{0x29, '~'},	// ` to ~
									{0x0C, '_'},    // - to _
									{0x0D, '+'},    // = to +
									{0x1A, '{'},    // [ to {
									{0x1B, '}'},    // ] to }
									{0x2B, '|'},    // \ to |
									{0x27, ':'},    // ; to :
									{0x28, '"'},    // ' to "
									{0x33, '<'},    // , to <
									{0x34, '>'},    // . to >
									{0x35, '?'}     // / to ?
};

/* flags for Shift, Control, and Caps Lock */
uint8_t ctrl = 0;
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
 * get_shifted_symbol(char keycode)
 *   Description: Returns the ascii code of shifted symbol if keycode is applicable
 *         Input: Keycode from keyboard input
 *        Output: Ascii_value of shifted symbol or 0 if keycode is not found
 *        Return: (char)ascii_value of shifted symbol or 0 if keycode is not found
 *  Side Effects: Looks up the table and return the ascii value if keycode is found in the table
 */
char get_shifted_symbol(char keycode) {
	uint8_t i;
	uint8_t ascii_value = 0;

	/* number of rows in the table 'shifted_symbols_map' */
	uint8_t table_row = sizeof(shifted_symbols_map)/sizeof(shifted_symbols_map[0]);

	for (i=0; i<table_row; i++) {
		if (keycode==shifted_symbols_map[i][0]) {		// check first column
			ascii_value = shifted_symbols_map[i][1];	// return second column
			break;
		}
	}
	/* returns 0 if keycode is not found */
	return ascii_value;
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
		
		/* if the keycode received is Enter, add the character to the line buffer and call the command */
		if ( keymap[(uint8_t)keycode] == '\n' ) {
			putc('\n');
			buffer_command();
			clear_buffer();
			putc('\n');
			return;
		}

		/* if the keycode received is anything above F1 being pressed, don't do anything unless it's a release from a Shift key or Control key */
		if ( (uint8_t)keycode >= F1_P ) {
			if ( (uint8_t)keycode == LSHIFT_R || (uint8_t)keycode == RSHIFT_R ) {
				shift_pressed--;
			}
			if ( (uint8_t)keycode == CTRL_R ) {
				ctrl--;
			}
			return;
		}
		
		/* if the keycode received is Left Alt, don't do anything */
		if ( (uint8_t)keycode == LALT )
			return;

		/* if the key that's pressed is Control, set its flag to 1 */
		if ( (uint8_t)keycode == CTRL_P ) {
			ctrl++;
			return;
		}
		
		/* if the key that's pressed is Backspace, clear the previously entered symbol and move the cursor back */
		if ((uint8_t)keycode == BACKSPACE) {
			/* if the line buffer is empty, don't do anything */
			if (buffer_count == 0)
				return;
			remove_from_buffer();
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
		
		/* if the key that's pressed is Left Shift or Right Shift, toggle the shift_pressed flag */
		if ((uint8_t)keycode == LSHIFT_P || (uint8_t)keycode == RSHIFT_P) {
			shift_pressed++;
			return;
		}
		
		/* if ctrl is currently being pressed and the character being entered is 'l', clear the screen and put the cursor on top */
		if ( ctrl > 0 && keymap[(uint8_t)keycode] == 'l' ) {
			clear_buffer();
			ctrl_l();
			return;
		}
		
		/* if the line buffer is full, don't do anything */
		if (buffer_count == LINE_BUFFER_SIZE-1)
			return;

		/* if shift is being pressed and the key is symbol or number, look up that symbol and print */
		if (shift_pressed) {
			char found_symbol = get_shifted_symbol(keycode);
			if (found_symbol>0) {	// if symbol found
				type_to_buffer(found_symbol);
				putc(found_symbol);
				return;
			}
		}

        /* prints the pressed key on screen and stores the character in the line buffer while checking if Caps Lock has been toggled and/or if Shift is being pressed */
        if (caps_lock^shift_pressed>0) {
			if ( (((uint8_t)keycode >= Q_MAP) && ((uint8_t)keycode <= P_MAP)) || (((uint8_t)keycode >= A_MAP) && ((uint8_t)keycode <= L_MAP)) || (((uint8_t)keycode >= Z_MAP) && ((uint8_t)keycode <= M_MAP)) ) {
				type_to_buffer(keymap[(uint8_t)keycode]-CAP_OFFSET);
				putc(keymap[(uint8_t)keycode]-CAP_OFFSET);
				return;
			}
		}
		type_to_buffer(keymap[(uint8_t)keycode]);
		putc(keymap[(uint8_t)keycode]);
    }
}
