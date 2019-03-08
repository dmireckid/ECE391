#include "tests.h"
#include "x86_desc.h"
#include "lib.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

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

/* Paging Test - Example
 * 
 * Asserts that video memory and kernel memory are paged properly
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Coverage: , IDT definition
 * Files: paging.h/c, paging.h/S
 */
void paging_test(){

	char * kernel_memory_start = (char*) 0x0400000;
	char * kernel_memory_end = (char*)0x07fffff;
	char * kernel_memory = (char*)0x04abcde;

	char * video_memory_start = (char*)0x000b8000;
	char * video_memory_end =(char*) 0x000b8fff;
	char * video_memory = (char*)0x000b8abc;
	
	/*char * bad_video_memory_start = (char*)0x000b7fff;
	printf("bad video memory start: %x\n" , * bad_video_memory_start);*/
	/*char * bad_video_memory_end = (char*)0x000b9000;
	printf("bad video memory end: %x\n" , * bad_video_memory_end);*/
	/*char * bad_kernel_memory_start = (char*)0x03fffff;
	printf("bad kernel memory start: %x\n" , * bad_kernel_memory_start);*/
	/*char * bad_kernel_memory_end = (char*)0x0800000;
	printf("bad kernel memory end: %x\n" , * bad_kernel_memory_end);*/


	printf("data at beginning of kernel memory: %x\n" , * kernel_memory_start);

	printf("data at end of kernel memory: %x\n" , * kernel_memory_end);
	
	printf("data inside of kernel memory: %x\n" , * kernel_memory);
	
	printf("data at beginning of video memory: %x\n" , * video_memory_start);
	
	printf("data at end of video memory: %x\n" , * video_memory_end);
	
	printf("data inside of video memory: %x\n" , * video_memory);

	int a = 5; int* a_ptr = &a;

	printf("a_ptr: %x \n",a_ptr);

	printf("a_ptr deref: %u \n",*a_ptr);
}

/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	//TEST_OUTPUT("idt_test", idt_test());
	// launch your tests here
	paging_test();
}
