/* PIT HEADER FILE */

#include "types.h"

#define CHANNEL0 0x40
#define CHANNEL1 0x41
#define CHANNEL2 0x42
#define PIT_PORT 0x43
#define PIT_MODE 0x36  //MODE 3 (SQUARE WAVE)
#define PIT_IRQ 0
#define PIT_FREQ 39773 //30 Hz
#define PIT_MASK 0xFF
#define SHIFT_8 8


uint32_t PIT_terminal;

void init_pit();
void pit_handler_function();


