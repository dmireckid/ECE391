.text

.globl keyboard_handler

keyboard_handler:
    # should we save flags?
    pushal
    pushfl
    call keyboard_handler_function
    popfl
    popal
    iret
