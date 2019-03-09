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

	if (directory_entry_array[0].val != 0x3+0x20+(int)table_entry_array)
		return FAIL;

	if (directory_entry_array[1].val != 0x83+0x60+KERNEL_ADDR)
		return FAIL;
	
	if (table_entry_array[VIDEO_ADDR/KB_4].val != 0x3+0x60+VIDEO_ADDR)
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

	/* check to see if the memory address right before the start of video memory (0xb7fff) causes a page fault */
	//puts("Testing dereferenced pointer before start of video memory: \n");
	//printf("%x\n", * (char*)0x000b7fff);
	
	/* check to see if the memory address right after the end of video memory (0xb9000) causes a page fault */
	//puts("Testing dereferenced pointer after end of video memory: \n");
	//printf("%x\n", * (char*)0x000b9000);
	
	/* check to see if the memory address right before the beginning of kernel memory (0x3fffff) causes a page fault */
	//puts("Testing dereferenced pointer before start of kernel memory: \n");
	//printf("%x\n", * (char*)0x03fffff);
	
	/* check to see if the memory address right after the end of kernel memory (0x800000) causes a page fault */
	//puts("Testing dereferenced pointer after end of kernel memory: \n");
	//printf("%x\n", * (char*)0x0800000);
	
	/* check to see if a NULL pointer returns a page fault */
	//puts("Testing dereferenced NULL pointer: \n");
	//printf("bad pointer: %d\n", * (int*)NULL);


	char * kernel_memory_start = (char*) 0x0400000;
	char * kernel_memory_end = (char*)0x07fffff;
	char * kernel_memory = (char*)0x04abcde;

	char * video_memory_start = (char*)0x000b8000;
	char * video_memory_end =(char*) 0x000b8fff;
	char * video_memory = (char*)0x000b8abc;

	printf("data at beginning of kernel memory: %x\n" , * kernel_memory_start);

	printf("data at end of kernel memory: %x\n" , * kernel_memory_end);
	
	printf("data inside of kernel memory: %x\n" , * kernel_memory);
	
	printf("data at beginning of video memory: %x\n" , * video_memory_start);
	
	printf("data at end of video memory: %x\n" , * video_memory_end);
	
	printf("data inside of video memory: %x\n" , * video_memory);

	int a = 5; int* a_ptr = &a;

	printf("a_ptr: %x \n",a_ptr);

	printf("a_ptr deref: %u \n",*a_ptr);
	
	return PASS;
}

/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	TEST_OUTPUT("idt_test", idt_test());
	// launch your tests here
	TEST_OUTPUT("paging_struct_test", paging_struct_test());
	TEST_OUTPUT("paging_test", paging_test());
}
