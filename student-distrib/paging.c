/* paging.c - Functions to help initialize paging
 */

#include "paging.h"
#include "paging_assem.h"

/*
 * initialize_page
 *   DESCRIPTION: Initializes paging in the kernel.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: initializes and enables paging
 */
void initialize_page() {
	
	int j;
	/* set all values in all page directory entries and table entries to 0 */
	for (j = 0; j < NUM_ENTRIES; j++) {
		directory_entry_array[j].val = 0;
		table_entry_array[j].val = 0;
	}
	
	/* video memory is at 0xB8000, in other words in index 0 of the directory_entry_array */
	directory_entry_array[0].present = 1;
	directory_entry_array[0].read_write = 1;
	directory_entry_array[0].page_size = 0;
	directory_entry_array[0].p_table_addr = ((int)table_entry_array)>>12;	// address has to be mapped from 4 kB, so shift physical address to right by 12
	table_entry_array[VIDEO_ADDR/KB_4].present = 1;
	table_entry_array[VIDEO_ADDR/KB_4].read_write = 1;
	table_entry_array[VIDEO_ADDR/KB_4].p_base_addr = VIDEO_ADDR/KB_4;		// address has to be mapped from 4 kB

	/* kernel space is at 4 MB (0x400000), in other words in index 1 of the directory_entry_array */
	directory_entry_array[1].present = 1;
	directory_entry_array[1].read_write = 1;
	directory_entry_array[1].page_size = 1;									// kernel space is 4 MB
	directory_entry_array[1].p_table_addr = KERNEL_ADDR>>12;				// address has to be mapped from 4 MB, so shift physical address to right by 12 (other 10 bits accomodated by reserved and PAT bits)
	
	/* enable paging through assembly linkage */
	enable_paging(directory_entry_array);
}
