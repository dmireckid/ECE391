#define ASM 1
#include "keyboard.h"

.globl keyboard_handler

keyboard_handler:
    # should we save flags?
    call keyboard_handler_function
    iret
