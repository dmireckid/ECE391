#include "idt_init.h"
#include "x86_desc.h"
#include "interrupt_handlers.h"
#include "lib.h"

#include "keyboard_asm.h"
#include "rtc_asm.h"

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
		idt[i].reserved1 = 0x1;			//initialize IDT values
		idt[i].size = 0x1;
		idt[i].reserved0 = 0x0;
		idt[i].dpl = 0x0;
		idt[i].present = 0x1;
		
		if(i == 0x80){		//0x80 is reserved for syscalls
			idt[i].dpl = 0x3;  //user-level
		}
		
		if(i >= 32){
			idt[i].reserved3 = 0x0;
			SET_IDT_ENTRY(idt[i], functions[20]);
		}
		
	}
	for(i=0; i<20; i++){			//interrupt handlers
		if(i==15){
			continue;
		}
		else{
			SET_IDT_ENTRY(idt[i], functions[i]);
		}
	}
	//SET_IDT_ENTRY(idt[0x20], );		//PIC handler
	SET_IDT_ENTRY(idt[0x21], keyboard_handler);	//keyboard handler
	SET_IDT_ENTRY(idt[0x28], rtc_handler); 		//RTC handler
	//SET_IDT_ENTRY(idt[0x80], syscall_handler);		//syscall handler
}

void divide_error(){
	printf("Divide error");
	while(1);
}

void debug(){
	printf("Debug error");
	while(1);
}

void nmi(){
	printf("NMI interrupt");
	while(1);
}

void breakpoint(){
	printf("Breakpoint");
	while(1);
}

void overflow(){
	printf("Overflow");
	while(1);
}

void bound_range(){
	printf("BOUND range exceeded");
	while(1);
}

void invalid_op(){
	printf("Invalid opcode");
	while(1);
}

void device_na(){
	printf("Device not available");
	while(1);
}

void double_fault(){
	printf("Double fault");
	while(1);
}

void seg_overrun(){
	printf("Coprocessor segment overrun");
	while(1);
}

void invalid_tss(){
	printf("Invalid TSS");
	while(1);
}

void seg_np(){
	printf("Segment not present");
	while(1);
}

void seg_fault(){
	printf("Stack-segment fault");
	while(1);
}

void gen_prot(){
	printf("General protection");
	while(1);
}

void page_fault(){
	printf("Page fault");
	while(1);
}

void blank(){
}

void fpe(){
	printf("x87 FPU floating-point error");
	while(1);
}

void align(){
	printf("Alignment check");
	while(1);
}

void machine(){
	printf("Machine check");
	while(1);
}

void simd(){
	printf("SIMD floating-point exception");
	while(1);
}

void general(){
	cli();
	printf("General interrupt");
	sti();
}


