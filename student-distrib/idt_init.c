#include "idt_init.h"
#include "x86_desc.h"
#include "lib.h"

#include "keyboard_asm.h"
#include "rtc_asm.h"

#define RTC_VAL 0x28
#define KEYBOARD_VAL 0x21

/*
 * idt_init
 *   DESCRIPTION: Initializes the IDT to proper values.
 *   INPUTS: NONE
 *   OUTPUTS: NONE
 *   RETURN VALUE: NONE
 *   SIDE EFFECTS: Exceptions and interrupts will have DPL 0
 *					SysCalls will have DPL 3
 *					Exceptions will use TRAP gates
 *					Interrupts will use INT gates
 *					Exceptions will print error messages
 *					Interrupts are linked to their proper handlers.
 */
 
void idt_init(){
	static void (*functions[21]) = {divide_error, debug, nmi, breakpoint, overflow, bound_range, 
									invalid_op, device_na, double_fault, seg_overrun, invalid_tss,
									seg_np, seg_fault, gen_prot, page_fault, blank, fpe, align,
									machine, simd, general}; //insert all handler functions
	int i;
	for(i=0; i<NUM_VEC; i++){			//loop through all IDT entries
		idt[i].seg_selector = KERNEL_CS;
		idt[i].reserved4 = 0x000;
		idt[i].reserved3 = 0x1;
		idt[i].reserved2 = 0x1;
		idt[i].reserved1 = 0x1;			//initialize IDT values to trap gate
		idt[i].size = 0x1;
		idt[i].reserved0 = 0x0;
		idt[i].dpl = 0x0;			//highest priority for exceptions
		idt[i].present = 0x1;
		
		if(i == 0x80){		//0x80 is reserved for syscalls
			idt[i].dpl = 0x3;  //user-level priority for sys calls
		}
		
		if(i >= 32){
			idt[i].reserved3 = 0x0;		//change to INT gate
			//SET_IDT_ENTRY(idt[i], functions[20]);
		}
		
	}
	for(i=0; i<20; i++){			//interrupt handlers
		if(i==15){
			continue;
		}
		else{
			SET_IDT_ENTRY(idt[i], functions[i]);		//set exception entries to their corresponding printing functions
		}
	}
	//SET_IDT_ENTRY(idt[0x20], PIT_handler);		//PIC timer handler
	SET_IDT_ENTRY(idt[KEYBOARD_VAL], keyboard_handler);	//keyboard handler
	SET_IDT_ENTRY(idt[RTC_VAL], rtc_handler); 		//RTC handler
	//SET_IDT_ENTRY(idt[0x80], syscall_handler);		//syscall handler
}


/*
 * exceptions
 *   DESCRIPTION: Exception handlers to print error message and
 *					enter infinite loop to prevent any more actions.
 *   INPUTS: NONE
 *   OUTPUTS: NONE
 *   RETURN VALUE: NONE
 *   SIDE EFFECTS: Prints individual error messages corresponding
 *					To the error received.
 */
void divide_error(){
	cli();
	clear();				//clear the screen
	printf("Divide error");	//print error message
	while(1);				//infinite loop
}

void debug(){
	cli();
	clear();					//clear the screen
	printf("Debug error");      //print error message
	while(1);                   //infinite loop
}

void nmi(){
	cli();
	clear();						//clear the screen
	printf("NMI interrupt");        //print error message
	while(1);                       //infinite loop
}

void breakpoint(){
	cli();
	clear();						//clear the screen
	printf("Breakpoint");           //print error message
	while(1);                       //infinite loop
}

void overflow(){
	cli();
	clear();				//clear the screen
	printf("Overflow");     //print error message
	while(1);               //infinite loop
}

void bound_range(){
	cli();
	clear();							//clear the screen
	printf("BOUND range exceeded");     //print error message
	while(1);                           //infinite loop
}

void invalid_op(){
	cli();
	clear();					//clear the screen
	printf("Invalid opcode");   //print error message
	while(1);                   //infinite loop
}

void device_na(){
	cli();
	clear();							//clear the screen
	printf("Device not available");     //print error message
	while(1);                           //infinite loop
}

void double_fault(){
	cli();
	clear();					//clear the screen
	printf("Double fault");     //print error message
	while(1);                   //infinite loop
}

void seg_overrun(){
	cli();
	clear();									//clear the screen
	printf("Coprocessor segment overrun");      //print error message
	while(1);                                   //infinite loop
}

void invalid_tss(){
	cli();
	clear();					//clear the screen
	printf("Invalid TSS");      //print error message
	while(1);                   //infinite loop
}

void seg_np(){
	cli();
	clear();							//clear the screen
	printf("Segment not present");      //print error message
	while(1);                           //infinite loop
}

void seg_fault(){
	cli();
	clear();						//clear the screen
	printf("Stack-segment fault");  //print error message
	while(1);                       //infinite loop
}

void gen_prot(){
	cli();
	clear();							//clear the screen
	printf("General protection");       //print error message
	while(1);                           //infinite loop
}

void page_fault(){
	cli();
	clear();					//clear the screen
	printf("Page fault");       //print error message
	while(1);                   //infinite loop
}

void blank(){
}

void fpe(){
	cli();
	clear();									//clear the screen
	printf("x87 FPU floating-point error");     //print error message
	while(1);                                   //infinite loop
}

void align(){
	cli();
	clear();					//clear the screen
	printf("Alignment check");  //print error message
	while(1);                   //infinite loop
}

void machine(){
	cli();
	clear();					//clear the screen
	printf("Machine check");    //print error message
	while(1);                   //infinite loop
}

void simd(){
	cli();
	clear();									//clear the screen
	printf("SIMD floating-point exception");    //print error message
	while(1);                                   //infinite loop
}

void general(){

}


