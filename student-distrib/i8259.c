/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* 
 * i8259_init()
 *   Description: Initialize the 8259 PIC 
 *         Input: None
 *        Output: None
 *        Return: None
 *  Side Effects: Initializes both PIC chips while preserving the mask registers
 */

void i8259_init(void) {
    /* Interrupt masks to determine which interrupts are enabled and disabled */
    uint8_t master_mask; /* IRQs 0-7  */
    uint8_t slave_mask;  /* IRQs 8-15 */

    outb(0xFF, MASTER_DATA);            //mask (close) all IRQs
    outb(0xFF, SLAVE_DATA);

    master_mask = inb(MASTER_DATA);     //save current masks
    slave_mask = inb(SLAVE_DATA);

    outb(ICW1, MASTER_COMMAND);         //enter init mode (0x11)
    outb(ICW1, SLAVE_COMMAND);

    outb(ICW2_MASTER, MASTER_DATA);     //set vector offsets (0x20 and 0x28)
    outb(ICW2_SLAVE, SLAVE_DATA);

    outb(ICW3_MASTER, MASTER_DATA);     //tell master that slave is at IRQ2 (0x04)
    outb(ICW3_SLAVE, SLAVE_DATA);       //tell slave that it has master

    outb(ICW4, MASTER_DATA);            //give info about environment
    outb(ICW4, SLAVE_DATA);

    outb(master_mask, MASTER_DATA);     //restore masks
    outb(slave_mask, SLAVE_DATA);
}

/* 
 * enable_irq()
 *   Description: Enables the specified IRQ 
 *         Input: The IRQ number
 *        Output: None
 *        Return: None
 *  Side Effects: Enables the specified IRQ number by writing a new mask register value
 */

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
    outb(mask_new, select);                 //(1<<irq_num) is 0x11111101 if irq_num is 1

    if(select==SLAVE_DATA && mask_prev==0xFF) {
        /* if this is the first time enabling anything on SLAVE, enable IRQ on MASTER where SLAVE is connected */
        enable_irq(SLAVE_IRQ_ON_MASTER);
	}
}

/* 
 * disable_irq()
 *   Description: Disables the specified IRQ 
 *         Input: The IRQ number
 *        Output: None
 *        Return: None
 *  Side Effects: Disables the specified IRQ number by writing a new mask register value
 */

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
    outb(mask_new, select);                 //(1<<irq_num) is 0x00000010 if irq_num is 1

    if(select==SLAVE_DATA && mask_new==0xFF) {
        /* if there is nothing enabled on SLAVE, disable IRQ on MASTER where SLAVE is connected */
        disable_irq(SLAVE_IRQ_ON_MASTER);
	}
}

/* 
 * send_eoi()
 *   Description: Sends EOI signal to the specified IRQ number 
 *         Input: The IRQ number
 *        Output: None
 *        Return: None
 *  Side Effects: Sends EOI signal to the IRQ number correcly according to the location of the IRQ
 */

void send_eoi(uint32_t irq_num) {
    if (irq_num>=8) {
        outb(EOI|(irq_num-8), SLAVE_COMMAND);           //sends EOI signal to SLAVE if irq is on slave
        outb(EOI|SLAVE_IRQ_ON_MASTER, MASTER_COMMAND);  //sends EOI signal to MASTER IRQ where slave is
    } else {
        outb(EOI|irq_num, MASTER_COMMAND);              //sends EOI signal to MASTER if irq is on master
    }
}
