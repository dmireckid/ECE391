/* rtc.c - RTC init and interrupt
 */
#include "rtc.h"

#include "i8259.h"
#include "lib.h"
#include "types.h"
//https://wiki.osdev.org/RTC

#define RTC_MAR	0x70	//address Register / Index register
#define RTC_MDR	0x71	//Data Register / CMOS register
#define RTC_IRQ_LINE 8

//
#define RTC_REGISTER_A	0x0A
#define	RTC_REGISTER_B	0x0B
#define	RTC_REGISTER_C	0x0C
#define	RTC_REGISTER_D	0x0D

#define A_turn_on 0x20 //control register A turn on osciallator
#define A_2_Hz 0x0F //control register A 2 Hz setting
#define init_B 0x40 //control register B Interrupt enable setting

void init_rtc(void){

	

	
	//turn on oscillator
	uint8_t reg_a = A_turn_on;
	outb( RTC_REGISTER_A, RTC_MAR);
	outb( reg_a,RTC_MDR);

	//set oscillator frequency
	outb( RTC_REGISTER_A, RTC_MAR);
	reg_a = (uint8_t)inb(RTC_MDR);
	reg_a |= A_2_Hz;
	outb( RTC_REGISTER_A, RTC_MAR);
	outb( reg_a,RTC_MDR);
	
	
	
	outb( RTC_REGISTER_B,RTC_MAR);
	uint8_t reg_b = (uint8_t)inb(RTC_MDR);
	//toggle the interrupt enable bit
	reg_b |= init_B;
	outb( RTC_REGISTER_B,RTC_MAR);
	outb(reg_b,RTC_MDR);
	
	
	
	
	enable_irq(RTC_IRQ_LINE);
	
}


void rtc_interrupt(void){
	//read register C so interrupt is cleared from the RTC
	outb( RTC_REGISTER_C, RTC_MAR);
	inb(RTC_MDR);
	//required functionality for cp1
	send_eoi(RTC_IRQ_LINE);
	test_interrupts();

}
