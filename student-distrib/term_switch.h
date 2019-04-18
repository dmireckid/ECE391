/* term_switch.h - terminal switches
 */
 
#ifndef _TERM_SWITCH_H
#define _TERM_SWITCH_H
#include "types.h"
#include "term_driver.h"

#define SHELL1	1
#define SHELL2	2
#define SHELL3	3

uint8_t curr_term;


typedef struct __attribute__((packed)) term{

    uint8_t keyboard[LINE_BUFFER_SIZE];
	uint8_t buf_count;
    uint8_t screenx;
    uint8_t screeny;
	uint8_t curr_pid;

}term_t;

void switch_terminal(uint8_t keycode);

#endif
