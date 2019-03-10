#include "tests.h"
#include "x86_desc.h"
#include "lib.h"

#include "paging.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

/*
 * assertion_failure
 *   DESCRIPTION: Directly call an entry in the IDT
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: causes an exception or interrupt
 */
static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

// add more tests here

/* Garbage Input/Output Test
 * 
 * Asserts that incorrect inputs/outputs produce errors/exceptions
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Coverage: Garbage inputs/outputs
 * Files: idt_init.h/c
 */
 int division_test(){
	 volatile int a=1;
	 volatile int b=0;
	 volatile int c=a/b;	//attempt to divide by zero to throw an exception
	 return c;
 }

/* IDT Value Test
 * 
 * Asserts that all IDT entries contain values
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Coverage: IDT values
 * Files: idt_init.h/c
 */
int idt_val_test(){
	TEST_HEADER;
	
	int i;
	for (i = 0; i < NUM_VEC; i++) {
		if (idt[i].val == 0)
			return FAIL;
	}
	
	return PASS;
}

/* Paging Struct Test
 * 
 * Asserts that paging structure values are correct
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Coverage: Paging structure values
 * Files: paging.h/c
 */
int paging_struct_test(){
	TEST_HEADER;

	if (directory_entry_array[0].val != DIREC_0_BITS+(int)table_entry_array)
		return FAIL;

	if (directory_entry_array[1].val != DIREC_1_BITS+KERNEL_ADDR)
		return FAIL;
	
	if (table_entry_array[VIDEO_ADDR/KB_4].val != TABLE_23_BITS+VIDEO_ADDR)
		return FAIL;

	int i;
	for (i = 0; i < NUM_ENTRIES; i++) {
		if (i > 1 && directory_entry_array[i].val != 0)
			return FAIL;
		if (i != VIDEO_ADDR/KB_4 && table_entry_array[i].val != 0)
			return FAIL;
	}
	
	return PASS;
}

/* Paging Test
 * 
 * Asserts that video memory and kernel memory are paged properly
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Coverage: Paged addresses
 * Files: paging.h/c, paging.h/S
 */
int paging_test(){
	TEST_HEADER;

	/* uncomment to check if the memory address right before the start of video memory (0xb7fff) causes a page fault */
	//printf("%x\n", * (char*)(VIDEO_ADDR_S-1));
	
	/* uncomment to check if the memory address right after the end of video memory (0xb9000) causes a page fault */
	//printf("%x\n", * (char*)(VIDEO_ADDR_E+1));
	
	/* uncomment to check if the memory address right before the beginning of kernel memory (0x3fffff) causes a page fault */
	//printf("%x\n", * (char*)(KERNEL_ADDR_S-1));
	
	/* uncomment to check if the memory address right after the end of kernel memory (0x800000) causes a page fault */
	//printf("%x\n", * (char*)(KERNEL_ADDR_E+1));
	
	/* uncomment to check if a NULL pointer returns a page fault */
	//printf("bad pointer: %d\n", * (int*)NULL);


	/* check to see if addresses at beginning, middle, and end of kernel and video memory are paged */
	char * kernel_memory_start = (char*)KERNEL_ADDR_S;
	char * kernel_memory_end = (char*)KERNEL_ADDR_E;
	char * kernel_memory = (char*)KERNEL_ADDR_M;

	char * video_memory_start = (char*)VIDEO_ADDR_S;
	char * video_memory_end =(char*)VIDEO_ADDR_E;
	char * video_memory = (char*)VIDEO_ADDR_M;

	printf("data at beginning of kernel memory: %x\n" , * kernel_memory_start);
	printf("data at end of kernel memory: %x\n" , * kernel_memory_end);
	printf("data inside of kernel memory: %x\n" , * kernel_memory);
	printf("data at beginning of video memory: %x\n" , * video_memory_start);
	printf("data at end of video memory: %x\n" , * video_memory_end);
	printf("data inside of video memory: %x\n" , * video_memory);


	/* check to see if an initialized pointer lies within the kernel memory */
	int a = FIVE; int* a_ptr = &a;
	printf("a_ptr: %x \n",a_ptr);
	printf("a_ptr deref: %u \n",*a_ptr);


	return PASS;
}

/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/*
 * launch_tests
 *   DESCRIPTION: Launch our test cases to prove that our code works
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: executes our test functions and returns FAIL or PASS for each one
 */
void launch_tests(){
	TEST_OUTPUT("idt_test", idt_test());
	// launch your tests here
	//division_test();
	//asm volatile("int $19");
	TEST_OUTPUT("idt_val_test", idt_val_test());
	TEST_OUTPUT("paging_struct_test", paging_struct_test());
	TEST_OUTPUT("paging_test", paging_test());
}
