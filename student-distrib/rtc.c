/* rtc.c - RTC init and interrupt
 */


#include "i8259.h"
#include "lib.h"
#include "rtc.h"

//https://wiki.osdev.org/RTC

#define RTC_MAR	0x70	//address Register / Index register
#define RTC_MDR	0x71	//Data Register / CMOS register
#define RTC_IRQ_LINE 8

//
#define RTC_REGISTER_A	0x8A
#define	RTC_REGISTER_B	0x8B
#define	RTC_REGISTER_C	0x8C
#define	RTC_REGISTER_D	0x8D


#define 2_Hz_A 0x2F //control register A 2 Hz setting,turn oscillator on
#define init_B 0x40 //control register B Interrupt enable setting

void init_rtc(){


	
	
	outb( RTC_REGISTER_A, RTC_MAR);
	char reg_a = inb(RTC_MDR);
	
	//toggle the bottome 7 bits 
	reg_a &= 0x80;
	reg_a |= 2_Hz_A;
	
	outb( reg_a,RTC_MDR);
	
	
	
	outb( RTC_REGISTER_B,RTC_MAR);
	char reg_b = inb(RTC_MDR);
	//toggle the interrupt enable bit
	reg_b |= init_B;
	
	outb(reg_b,RTC_MDR);
	
	
	
	enable_irq(RTC_IRQ_LINE);
	
	

}


void rtc_interrupt(){
	
	send_eoi(RTC_IRQ_LINE);
	printf("rtc interrupt recieved \n" );

}