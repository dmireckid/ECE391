/* term_switch.h - terminal switches
 */
 
#ifndef _TERM_SWITCH_H
#define _TERM_SWITCH_H
#include "types.h"

#define SHELL1	1
#define SHELL2	2
#define SHELL3	3

uint8_t curr_term;

void switch_terminal(uint8_t keycode);

#endif
