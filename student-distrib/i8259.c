/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void i8259_init(void) {
    master_mask = inb(MASTER_DATA);     //save current masks
    slave_mask = inb(SLAVE_DATA);

    outb(MASTER_COMMAND, ICW1);         //enter init mode (0x11)
    outb(SLAVE_COMMAND, ICW1);

    outb(MASTER_DATA, ICW2_MASTER);     //set vector offsets (0x20 and 0x28)
    outb(SLAVE_DATA, ICW2_SLAVE);

    outb(MASTER_DATA, ICW3_MASTER);     //tell master that slave is at IRQ2 (0x04)
    outb(SLAVE_DATA, ICW3_SLAVE);       //tell slave that it has master

    outb(MASTER_DATA, ICW4);            //give info about environment
    outb(SLAVE_DATA, ICW4);

    outb(MASTER_DATA, master_mask);     //restore masks
    outb(SLAVE_DATA, slave_mask);
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    uint16_t    select;                     //port number for desired PIC
    uint8_t     mask_prev, mask_new;        //8-bit mask register value

    if (irq_num<8) {
        select = MASTER_DATA;               //IRQ is on MASTER, so select MASTER
    } else {
        select = SLAVE_DATA;                //IRQ is on SLAVE, so select SLAVE
        irq_num -= 8;                       //subtract 8 from irq_num for correct offset
    }

    mask_prev = inb(select);
    mask_new = mask_prev & ~(1<<irq_num);   //unmask irq_num by making the value zero
    outb(select, mask_new);                 //ie. 0xXXXXXX0X if irq_num is 1
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
    uint16_t    select;                     //port number for desired PIC
    uint8_t     mask_prev, mask_new;        //8-bit mask register value

    if (irq_num<8) {
        select = MASTER_DATA;               //IRQ is on MASTER, so select MASTER
    } else {
        select = SLAVE_DATA;                //IRQ is on SLAVE, so select SLAVE
        irq_num -= 8;                       //subtract 8 from irq_num for correct offset
    }

    mask_prev = inb(select);
    mask_new = mask_prev | (1<<irq_num);    //mask irq_num by making the value one
    outb(select, mask_new);                 //ie. 0xXXXXXX1X if irq_num is 1
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    if (irq_num>=8) {
        outb(SLAVE_COMMAND, EOI);   //sends EOI signal to SLAVE if irq is on slave
    }
    outb(MASTER_COMMAND, EOI);      //sends EOI signal to MASTER regardless of irq location
}
