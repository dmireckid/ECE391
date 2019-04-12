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

// rtc register addresses
#define RTC_REGISTER_A	0x0A
#define	RTC_REGISTER_B	0x0B
#define	RTC_REGISTER_C	0x0C
#define	RTC_REGISTER_D	0x0D



#define init_B 0x40 //control register B Interrupt enable setting


//toggle to 1 when interrupt handler is called, and toggle to 0 when rtc_read is called
static volatile int interrupt_occurred = 0;

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

	//set oscillator frequency
	outb( RTC_REGISTER_A, RTC_MAR);
	reg_a = (uint8_t)inb(RTC_MDR);
	reg_a |= A_2_Hz;//set bottom 4 bits
	outb( RTC_REGISTER_A, RTC_MAR);
	outb( reg_a,RTC_MDR);
	
	
	
	outb( RTC_REGISTER_B,RTC_MAR);
	uint8_t reg_b = (uint8_t)inb(RTC_MDR);
	//toggle the interrupt enable bit (bit 6)
	reg_b |= init_B;
	outb( RTC_REGISTER_B,RTC_MAR);
	outb(reg_b,RTC_MDR);
	
	
	
	interrupt_occurred=0;
	enable_irq(RTC_IRQ_LINE);
	
}

/*
 *	rtc_interrupt
 *  DESCRIPTION:called by rtc_handler which is the function pointed to in the IDT for IRQ line 8
 *	INPUTS:none
 *	OUTPUTS: sends eoi to pic, screws up the video memory by calling test interrupts
 *	RETURN VALUE:none
 *	SIDE EFFECTS: screws up video memory
 */
void rtc_interrupt(void){
	send_eoi(RTC_IRQ_LINE);
	
	//read register C so interrupt is cleared from the RTC
	outb( RTC_REGISTER_C, RTC_MAR);
	inb(RTC_MDR);
	//set off global so rtc_read can continue
	interrupt_occurred = 1;
	//required functionality for cp1
	//test_interrupts();
	
}


/*
 *	rtc_open
 *  DESCRIPTION:sets rtc clock to 2Hz
 *	INPUTS:the name of a file , represented as a array of bytes, with max size of 32 (addresses in filesys.c)
 *	OUTPUTS: none
 *	RETURN VALUE: 0 for success
 *  If the named file does not exist or no descriptors are free, the call returns -1
 *	SIDE EFFECTS: none
 */
int32_t rtc_open(const uint8_t* filename){
	 
	//set oscillator frequency
	outb( RTC_REGISTER_A, RTC_MAR);
	uint8_t reg_a = (uint8_t)inb(RTC_MDR);
	reg_a |= A_2_Hz;//set bottom 4 bits
	outb( RTC_REGISTER_A, RTC_MAR);
	outb( reg_a,RTC_MDR);
	interrupt_occurred = 0;
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
	return 0;
}

/*
 *	rtc_write
 *  DESCRIPTION:set the rate of periodic interrupts  
 * 
 *  in CP2, this rate is the same as the rate provided in buf 
 * 
 *  in a future CP, this rate will be 1024HZ
 *  and a global interrupt_rate array will track the rate assigned to each file descriptor
 *  
 *	INPUTS:
 *  int32_t fd - a file descriptor that was assigned in rtc_open
 *  const void* buf -  a pointer to an integer specifying the interrupt rate in Hz
 *  int32_t nbytes - number of bytes in buffer (must be 4)
 *	OUTPUTS: none
 *	RETURN VALUE:the number of bytes written, or -1 on failure
 *	SIDE EFFECTS: RTC may run faster 
 */

int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes){
	//number of bytes in buffer must be 4
	if (nbytes!=4) return -1;
	
	// this byte will contain the bottom 4 bits that have to be changed to control register A of the RTC
	volatile uint8_t reg_a_bottom_4 = 0;volatile uint8_t reg_a = 0;

	//the allowed frequencies are powers of two between 2 and 1024 
	//get the frequency
	volatile int32_t freq = *((int32_t*)buf);
	//check the frequency
	switch(freq)
	{
		case f_2_Hz:
			reg_a_bottom_4 = A_2_Hz;break;
		case f_4_Hz:
			reg_a_bottom_4 = A_4_Hz;break;
		case f_8_Hz:
			reg_a_bottom_4 = A_8_Hz;break;
		case f_16_Hz:
			reg_a_bottom_4 = A_16_Hz;break;
		case f_32_Hz:
			reg_a_bottom_4 = A_32_Hz;break;
		case f_64_Hz:
			reg_a_bottom_4 = A_64_Hz;break;
		case f_128_Hz:
			reg_a_bottom_4 = A_128_Hz;break;
		case f_256_Hz:
			reg_a_bottom_4 = A_256_Hz;break;
		case f_512_Hz:
			reg_a_bottom_4 = A_512_Hz;break;
		case f_1024_Hz:
			reg_a_bottom_4 = A_1024_Hz;break;
		default:
			return -1;
	}


	//set oscillator frequency
	outb( RTC_REGISTER_A, RTC_MAR);
	reg_a= 0xF0 & (uint8_t)inb(RTC_MDR); //get the top 4 bits only

	reg_a |= reg_a_bottom_4;//set bottom 4 bits

	outb( RTC_REGISTER_A, RTC_MAR);
	outb( reg_a,RTC_MDR);

	interrupt_occurred =0;


	return nbytes;

}



/*
 *	rtc_read
 *  DESCRIPTION:spins until a interrupt has occurred!
 * 
 *  in CP2, the rtc interrupt rate is the same as provided in rtc_write 
 * 
 *  in a future CP, the rtc interrupt rate will be 1024HZ
 *  and a global interrupt_rate array will track the rate assigned to each file descriptor
 *  then this method will count the number 1024Hz interrupts occur until the rate assigned to
 *	the file descriptor has occurred 
 *
 *	INPUTS:
 *  int32_t fd - a file descriptor (used in sys_calls.c)
 *  const void* buf -not used, but required for system call syntax
 *  int32_t nbytes - not used, but required for system call syntax
 *	OUTPUTS: none
 *	RETURN VALUE:the number of bytes written (0 for success), or -1 on failure
 *	SIDE EFFECTS: stalls a program
 */
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes)
{
	interrupt_occurred =0;
	while(interrupt_occurred==0);
	interrupt_occurred =0;
	return 0;

}



