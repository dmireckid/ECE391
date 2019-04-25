/* rtc.c - RTC init and interrupt
 */
#include "rtc.h"

#include "i8259.h"
#include "lib.h"
#include "types.h"
#include "pit.h"
//https://wiki.osdev.org/RTC

#define RTC_MAR	0x70	//address Register / Index register
#define RTC_MDR	0x71	//Data Register / CMOS register
#define RTC_IRQ_LINE 8

// rtc register addresses
#define RTC_REGISTER_A	0x0A
#define	RTC_REGISTER_B	0x0B
#define	RTC_REGISTER_C	0x0C
#define	RTC_REGISTER_D	0x0D

#define init_B 0x40 //control register B Interrupt enable setting
#define rtc_max_index 7


//toggle to 1 when interrupt handler is called, and toggle to 0 when rtc_read is called
static volatile int interrupt_occurred[rtc_max_index] = {0,0,0,0,0,0,0};
static volatile int rate[rtc_max_index]  = {f_2_Hz,f_2_Hz,f_2_Hz,f_2_Hz,f_2_Hz,f_2_Hz,f_2_Hz} ;
static volatile int count[rtc_max_index] = {0,0,0,0,0,0,0};


/*
 *	init_rtc
 *  DESCRIPTION: turns on the oscillator at 2 Hz, enable periodic interrupts
 *	INPUTS:none
 *	OUTPUTS: turns on the oscillator at 2 Hz, enable periodic interrupts
 *	RETURN VALUE:none
 *	SIDE EFFECTS: RTC interrupts happen
 */
void init_rtc(void){
	//turn on oscillator
	uint8_t reg_a = A_turn_on;
	outb( RTC_REGISTER_A, RTC_MAR);
	outb( reg_a,RTC_MDR);

	//set oscillator frequency to 256 (this NEVER changes due to rtc virtualization style)
	outb( RTC_REGISTER_A, RTC_MAR);
	reg_a = (uint8_t)inb(RTC_MDR);
	reg_a |= A_256_Hz;//set bottom 4 bits
	outb( RTC_REGISTER_A, RTC_MAR);
	outb( reg_a,RTC_MDR);

	outb( RTC_REGISTER_B,RTC_MAR);
	uint8_t reg_b = (uint8_t)inb(RTC_MDR);
	//toggle the interrupt enable bit (bit 6)
	reg_b |= init_B;
	outb( RTC_REGISTER_B,RTC_MAR);
	outb(reg_b,RTC_MDR);

	enable_irq(RTC_IRQ_LINE);
}

/*
 *	rtc_interrupt
 *  DESCRIPTION: called by rtc_handler which is the function pointed to in the IDT for IRQ line 8
 *	INPUTS: none
 *	OUTPUTS: sends eoi to pic, screws up the video memory by calling test interrupts
 *	RETURN VALUE: none
 *	SIDE EFFECTS: increases rtc counters of all programs
 */
void rtc_interrupt(void){
	send_eoi(RTC_IRQ_LINE);

	//read register C so interrupt is cleared from the RTC
	outb( RTC_REGISTER_C, RTC_MAR);
	inb(RTC_MDR);

	//go through every pid counter and increment it
	int i=0;
	for (i=0;i<=rtc_max_index;i++)
		count[i]++;
}

/*
 *	rtc_open
 *  DESCRIPTION: assigns 2Hz to a process's rtc
 *	INPUTS: filename - the name of a file , represented as a array of bytes, with max size of 32 (addresses in filesys.c)
 *	OUTPUTS: none
 *	RETURN VALUE: 0 for success, if the named file does not exist or no descriptors are free, return -1
 *	SIDE EFFECTS: none
 */
int32_t rtc_open(const uint8_t* filename){
	//set the rate of the program to 2 Hz
	rate[PIT_terminal] = f_2_Hz;
	//reset the program's count to 0 when RTC is opened
	count[PIT_terminal] = 0;
	return 0;
 }
 
/*
 *	rtc_close()
 *  DESCRIPTION: nothing for now
 *	INPUTS: file descriptor
 *	OUTPUTS: none
 *	RETURN VALUE:0 for success
 *	SIDE EFFECTS: none
 */
 int32_t rtc_close(int32_t fd){
	//simple reset, doesn't do anything
	rate[PIT_terminal] = f_2_Hz;
	count[PIT_terminal] = 0;
	return 0;
}

/*
 *	rtc_write
 *  DESCRIPTION: set the rate of periodic interrupts  
 *	INPUTS:
 *  		int32_t fd 		- a file descriptor that was assigned in rtc_open
 *  		const void* buf - a pointer to an integer specifying the interrupt rate in Hz
 *  		int32_t nbytes 	- number of bytes in buffer (must be 4)
 *	OUTPUTS: none
 *	RETURN VALUE:the number of bytes written, or -1 on failure
 *	SIDE EFFECTS: changes the RTC oscillator rate for a specific program
 */
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes){
	//number of bytes in buffer must be 4
	if (nbytes!=NUM_BYTES) return -1;

	//the allowed frequencies are powers of two between 2 and 1024 
	//get the frequency
	volatile int32_t freq = *((int32_t*)buf);
	//check the frequency
	switch(freq)
	{
		case f_2_Hz:
			rate[PIT_terminal] = f_2_Hz;break;
		case f_4_Hz:
			rate[PIT_terminal] = f_4_Hz;break;
		case f_8_Hz:
			rate[PIT_terminal] = f_8_Hz;break;
		case f_16_Hz:
			rate[PIT_terminal] = f_16_Hz;break;
		case f_32_Hz:
			rate[PIT_terminal] = f_32_Hz;break;
		case f_64_Hz:
			rate[PIT_terminal] = f_64_Hz;break;
		case f_128_Hz:
			rate[PIT_terminal] = f_128_Hz;break;
		case f_256_Hz:
			rate[PIT_terminal] = f_256_Hz;break;
		case f_512_Hz:
			rate[PIT_terminal] = f_512_Hz;break;
		case f_1024_Hz:
			rate[PIT_terminal] = f_1024_Hz;break;
		default:
			return -1;
	}

	return nbytes;
}

/*
 *	rtc_read
 *  DESCRIPTION: spins until an interrupt associated with the specific program's rate has occurred
 *	INPUTS:
 *  	int32_t fd 		- a file descriptor (used in sys_calls.c)
 *  	const void* buf - not used, but required for system call syntax
 *  	int32_t nbytes	- not used, but required for system call syntax
 *	OUTPUTS: none
 *	RETURN VALUE: the number of bytes written (0 for success), or -1 on failure
 *	SIDE EFFECTS: stalls a program until the specified # of ticks have occurred
 */
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes)
{
	//wait until the adequate number of ticks for that program has been reached
	while(count[PIT_terminal] < f_256_Hz/rate[PIT_terminal]);
	count[PIT_terminal] = 0;
	return 0;
}
