/* keyboard.c - Functions to initialize and handle keyboard interrupts
 * vim:ts=4 noexpandtab
 */

#include "keyboard.h"

/* enable IRQ1 to open keyboard interrupt */
void keyboard_init(void) {
    enable_irq(1);
}

/* executed at keyboard interrupt, prints out pressed key on screen */
void keyboard_handler_function(void) {
    /* contains keyboard status data */
    unsigned char status;

    /* contains keyboard keycode data */
    char keycode;

    /* signal EOI to allow further IRQs */
    send_eoi(KEYBOARD_IRQ);

    /* get status number */
    status = (char)(inb(KEYBOARD_STATUS));

    /* if status is zero, there is no data to read */
    if (status & 0x01) {
        /* get keycode */
        keycode = (char)(inb(KEYBOARD_DATA));
        /* do we need this? */
        if (keycode<0) return; 
        /* prints the pressed key on screen */
        putc(keymap[keycode]);
    }
}
