/* term_switch.h - terminal switches
 */
 
#ifndef _TERM_SWITCH_H
#define _TERM_SWITCH_H
#include "types.h"
#include "term_driver.h"

#define TERM_1	1
#define TERM_2	2
#define TERM_3	3

uint32_t curr_term_num;

typedef struct __attribute__((packed)) term{

    char keyboard[LINE_BUFFER_SIZE];
	uint8_t buf_count;
    uint8_t screenx;
    uint8_t screeny;
	uint8_t curr_pid;
    uint32_t esp;
    uint32_t ebp;

}term_t;

term_t terminal_array[TERM_3+1];

void init_terminal();
void schedule_terminal(uint32_t old_terminal);
void switch_terminal(uint8_t keycode);
void backstage(uint32_t term_num);

#endif
