#include "idt_init.h"
#include "x86_desc.h"
#include "lib.h"

#include "keyboard_asm.h"
#include "rtc_asm.h"
#include "sys_calls_asm.h"
#include "sys_calls.h"
#include "types.h"
#include "pit_asm.h"

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
	SET_IDT_ENTRY(idt[PIT_VAL], pit_handler);			//PIC timer handler
	SET_IDT_ENTRY(idt[KEYBOARD_VAL], keyboard_handler);	//keyboard handler
	SET_IDT_ENTRY(idt[RTC_VAL], rtc_handler); 			//RTC handler
	SET_IDT_ENTRY(idt[SYSCALL_VAL], sys_call_handler);	//syscall handler
}


/*
 * exceptions
 *   DESCRIPTION: Exception handlers to print error message and
 *					call halt to end program.
 *   INPUTS: NONE
 *   OUTPUTS: NONE
 *   RETURN VALUE: NONE
 *   SIDE EFFECTS: Prints individual error messages corresponding
 *					To the error received.
 */
void divide_error(){
	cli();
	clear();					//clear the screen
	printf("Divide error\n");	//print error message
	halt(EX_STATUS);			//call halt
}

void debug(){
	cli();
	clear();					//clear the screen
	printf("Debug error\n");	//print error message
	halt(EX_STATUS);			//call halt
}

void nmi(){
	cli();
	clear();					//clear the screen
	printf("NMI interrupt\n");	//print error message
	halt(EX_STATUS);			//call halt
}

void breakpoint(){
	cli();
	clear();				//clear the screen
	printf("Breakpoint\n");	//print error message
	halt(EX_STATUS);		//call halt
}

void overflow(){
	cli();
	clear();				//clear the screen
	printf("Overflow\n");	//print error message
	halt(EX_STATUS);		//call halt
}

void bound_range(){
	cli();
	clear();							//clear the screen
	printf("BOUND range exceeded\n");	//print error message
	halt(EX_STATUS);					//call halt
}

void invalid_op(){
	cli();
	clear();					//clear the screen
	printf("Invalid opcode\n");	//print error message
	halt(EX_STATUS);			//call halt
}

void device_na(){
	cli();
	clear();							//clear the screen
	printf("Device not available\n");	//print error message
	halt(EX_STATUS);					//call halt
}

void double_fault(){
	cli();
	clear();					//clear the screen
	printf("Double fault\n");	//print error message
	halt(EX_STATUS);			//call halt
}

void seg_overrun(){
	cli();
	clear();									//clear the screen
	printf("Coprocessor segment overrun\n");	//print error message
	halt(EX_STATUS);							//call halt
}

void invalid_tss(){
	cli();
	clear();					//clear the screen
	printf("Invalid TSS\n");	//print error message
	halt(EX_STATUS);			//call halt
}

void seg_np(){
	cli();
	clear();							//clear the screen
	printf("Segment not present\n");	//print error message
	halt(EX_STATUS);					//call halt
}

void seg_fault(){
	cli();
	clear();							//clear the screen
	printf("Stack-segment fault\n");	//print error message
	halt(EX_STATUS);					//call halt
}

void gen_prot(){
	cli();
	clear();						//clear the screen
	printf("General protection\n");	//print error message
	halt(EX_STATUS);				//call halt
}

void page_fault(){
	cli();
	clear();
	uint32_t addr = test_cr2();	

	printf("\n0x%x\nPage fault\n",addr);	//print error message
	halt(EX_STATUS);		//call halt
}

void blank(){
}

void fpe(){
	cli();
	clear();									//clear the screen
	printf("x87 FPU floating-point error\n");	//print error message
	halt(EX_STATUS);							//call halt
}

void align(){
	cli();
	clear();						//clear the screen
	printf("Alignment check\n");	//print error message
	halt(EX_STATUS);				//call halt
}

void machine(){
	cli();
	clear();					//clear the screen
	printf("Machine check\n");	//print error message
	halt(EX_STATUS);			//call halt
}

void simd(){
	cli();
	clear();									//clear the screen
	printf("SIMD floating-point exception\n");	//print error message
	halt(EX_STATUS);							//call halt
}

void general(){

}


