#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "types.h"
#include "paging.h"
#include "rtc.h"
#include "term_driver.h"
#include "filesys.h"

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

	/*	check to see if index 0 of the directory holds the correct bits: 
	*	Bits 0 (present), 1 (read/write), and 5 (accessed) should be set to 1
	*	The base address to paging table should be there as well
	*/
	if (directory_entry_array[0].val != DIREC_0_BITS+(int)table_entry_array)
		return FAIL;

	/*	check to see if index 1 of the directory holds the correct bits: 
	*	Bits 0 (present), 1 (read/write), 5 (accessed), 6 (dirty), and 7 (page size) should be set to 1
	*	The base physical address to kernel memory should be there as well
	*/
	if (directory_entry_array[1].val != DIREC_1_BITS+KERNEL_ADDR)
		return FAIL;

	/*	check to see if index 23 of the table holds the correct bits: 
	*	Bits 0 (present), 1 (read/write), 5 (accessed), and 6 (dirty) should be set to 1
	*	The base physical address to video memory should be there as well
	*/
	if (table_entry_array[VIDEO_ADDR/KB_4].val != TABLE_23_BITS+VIDEO_ADDR)
		return FAIL;

	/*	run through the entire page directory and page table to see that every other entry is set to 0 */
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

//helper functions for rtc test


static inline void freq_test_1(int freq, int fd)
{
	//int32_t* buf = &freq;
	rtc_write(fd,(void *)freq,4);
	int count;
	for(count = 0;count<9;count++) 
	{
		rtc_read(fd,(void *)freq,4);
		printf("%u ", freq);
	}
}


static inline void freq_test_2(int freq, int fd)
{
	//int32_t* buf = &freq;
	rtc_write(fd,(void *)freq,4);
	int count;
	for(count = 0;count<freq;count++) 
	{
		rtc_read(fd,(void *)freq,4);
	}
	printf("%u rtc interrupts at %u Hz \n",freq,freq);
}


/* RTC
 * 
 * Description: check that rtc can have different frequencies, check funtionality of rtc_read/rtc_write
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Coverage: rtc functionalty
 * Files: rtc.c/rtc.h
 */
 #define min_freq 2
 #define max_freq 1024
void rtc_test()
{
	printf("\n RTC TEST \n");
	int fd = rtc_open(NULL);

	int freq = min_freq;
	printf("\nRTC Test 1\n");
	
	for(freq = min_freq;freq<=max_freq;freq=freq*2 )
	{
		freq_test_1(freq,fd);
	}
	printf("\nRTC Test 2\n");
	for(freq = min_freq;freq<=max_freq;freq=freq*2 )
	{
		freq_test_2(freq,fd);
	}
	for(freq = max_freq;freq>=min_freq;freq=freq/2 )
	{
		freq_test_2(freq,fd);
	}


	
	rtc_close(fd);

}
/* 

 *Keyboard/terminal test
 * 
 * Description: check line buffer, terminal_write
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Coverage: terminal/keyboard functionality
 * Files: term_driver.c/term_driver.h
 */

void kt_test()
{

	char string1[500] = "terminal_write test";
	terminal_write(0,string1,500);
	printf("\ndone\n");
	
	char string2[25] = "out of bound string";
	terminal_write(0,string2,500);




}

/* 

 *Filesystem test (read by name)
 * 
 * Description: Reads a file by name and prints its contents/stats
 * Inputs: int arg (txt or exe file)
 * Outputs: int
 * Side Effects: PASS for success, FAIL for failure
 * Coverage: Filesystem read_by_name functionality
 * Files: filesys.c/filesys.h
 */
int filesys_test(int arg){
	clear();
	dentry_t test;
	int32_t retval;
	uint32_t size;
	if(arg == 1){
		size = 500;		//buffer size 500 for text file
	}
	if(arg == 2){
		size = 6000;	//buffer size 6000 for exe file
	}
	uint8_t buf[size];
	retval = read_dentry_by_name((uint8_t*)"frame1.txt", &test);	//read certain file by name
	if(retval == -1){
		return FAIL;
	}
	int i;
	printf("File name: ");
	for(i=0; i<FILENAME_LEN; i++){
		putc(test.filename[i]);
	}
	printf("\n");
	printf("File type: %d\n", test.filetype);
	printf("Inode #: %d\n", test.inode_num);		//print file stats

	retval = read_data(test.inode_num, 0, buf, size);
	printf("File size: %d\n", (int32_t)retval);
	if(retval >= 500){
		printf("First 250 characters of large file...\n");
		for(i=0; i<250; i++){
			putc(buf[i]);		//only print first 250 and last 250 characters
		}									//if file is too long
		printf("\n");
		printf("Last 250 characters of large file...\n");
		for(i= retval-250; i<retval; i++){
			putc(buf[i]);
		}
		return PASS;
	}
	else{
		for(i=0; i<retval; i++){
			putc(buf[i]);		//or print whole file if it is not too long
		}
	}
	//printf("returning");
	return PASS;
}

/* 

 *Filesystem test index (read by index)
 * 
 * Description: Reads a file by index and prints its contents/stats
 * Inputs: int arg (index)
 * Outputs: int
 * Side Effects: PASS for success, FAIL for failure
 * Coverage: Filesystem read_by_index functionality
 * Files: filesys.c/filesys.h
 */
int filesys_test_index(int arg){
	clear();
	dentry_t test;
	int32_t retval;
	uint32_t size;
	if(arg == 10 || arg == 14 || arg == 15){
		size = 500;		//buffer size is 500 if text file
	}
	else{
		size = 6000;	//6000 if executable file
	}
	uint8_t buf[size];
	retval = read_dentry_by_index(arg, &test);	//read the file at certain bootblock index
	if(retval == -1){
		return FAIL;
	}
	int i;
	printf("File name: ");
	for(i=0; i<FILENAME_LEN; i++){
		putc(test.filename[i]);
	}
	printf("\n");
	printf("File type: %d\n", test.filetype);
	printf("Inode #: %d\n", test.inode_num);		//print file stats

	retval = read_data(test.inode_num, 0, buf, size);
	printf("File size: %d\n", (int32_t)retval);
	if(retval >= 500){
		printf("First 250 characters of large file...\n");
		for(i=0; i<250; i++){
			putc(buf[i]);
		}
		printf("\n");
		printf("Last 250 characters of large file...\n");
		for(i= retval-250; i<retval; i++){
			putc(buf[i]);		//print shortened version of file if very long
		}
		return PASS;
	}
	else{
		for(i=0; i<retval; i++){
			putc(buf[i]);		//if not too long, print whole file
		}
	}
	//printf("returning");
	return PASS;
}

/* 

 *Filesystem test directory (read by name)
 * 
 * Description: Reads a directory and prints all files names in it
 * Inputs: NONE
 * Outputs: int
 * Side Effects: PASS for success, FAIL for failure
 * Coverage: Filesystem read_d functionality
 * Files: filesys.c/filesys.h
 */
int filesys_test_directory(){
	clear();
	uint8_t buf[FILENAME_LEN];
	int32_t i=0;
	while(read_d(i++, FILENAME_LEN, buf) != 0){		//print out the whole directory contents
		printf("%s\n", buf);
	}
	return PASS;
}

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

	//rtc_test();

	//kt_test();
	
	//filesys_test(2);
	filesys_test_index(8);
	//filesys_test_directory();
}
