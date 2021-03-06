/* paging.c - Functions to help initialize paging
 */

#include "paging.h"
#include "paging_assem.h"
#include "term_switch.h"

/*
 * initialize_page
 *   DESCRIPTION: Initializes paging in the kernel
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

	/*	video memory is at 0xB8000, in other words in index 0 of the directory_entry_array, so
	*	set the entry's present and read/write bits to 1 and page size to 0 since video memory
	*	is of size 4 kB
	*/
	directory_entry_array[0].present = 1;
	directory_entry_array[0].read_write = 1;
	directory_entry_array[0].page_size = 0;
	directory_entry_array[0].p_table_addr = ((int)table_entry_array)>>SHIFT_12;	// address has to be mapped from 4 kB, so shift physical address to right by 12

	/*	video memory is at 0xB8000, in other words in index VIDEO_ADDR/KB_4 of the table_entry_array, so
	*	set the entry's present and read/write bits to 1
	*/
	table_entry_array[VIDEO_ADDR/KB_4].present = 1;
	table_entry_array[VIDEO_ADDR/KB_4].read_write = 1;
	table_entry_array[VIDEO_ADDR/KB_4].p_base_addr = VIDEO_ADDR/KB_4;			// address has to be mapped from 4 kB

	/*	terminal 1 memory is at 0xB9000, in other words in index TERM_VID_1/KB_4 of the table_entry_array, so
	*	set the entry's present and re/write bits to 1
	*/
	table_entry_array[TERM_VID_1/KB_4].present = 1;
	table_entry_array[TERM_VID_1/KB_4].read_write = 1;
	table_entry_array[TERM_VID_1/KB_4].p_base_addr = TERM_VID_1/KB_4;			// address has to be mapped from 4 kB
	
	/*	terminal 2 memory is at 0xBA000, in other words in index TERM_VID_2/KB_4 of the table_entry_array, so
	*	set the entry's present and re/write bits to 1
	*/
	table_entry_array[TERM_VID_2/KB_4].present = 1;
	table_entry_array[TERM_VID_2/KB_4].read_write = 1;
	table_entry_array[TERM_VID_2/KB_4].p_base_addr = TERM_VID_2/KB_4;			// address has to be mapped from 4 kB
	
	/*	terminal 3 memory is at 0xBB000, in other words in index TERM_VID_3/KB_4 of the table_entry_array, so
	*	set the entry's present and re/write bits to 1
	*/
	table_entry_array[TERM_VID_3/KB_4].present = 1;
	table_entry_array[TERM_VID_3/KB_4].read_write = 1;
	table_entry_array[TERM_VID_3/KB_4].p_base_addr = TERM_VID_3/KB_4;			// address has to be mapped from 4 kB

	/*	kernel space is at 4 MB (0x400000), in other words in index 1 of the directory_entry_array, so
	*	set the entry's present and read/write bits to 1 and page size to 1 since kernel memory
	*	is of size 4 MB
	*/
	directory_entry_array[1].present = 1;
	directory_entry_array[1].read_write = 1;
	directory_entry_array[1].page_size = 1;
	directory_entry_array[1].p_table_addr = KERNEL_ADDR>>SHIFT_12;				// address has to be mapped from 4 MB, so shift physical address to right by 12 (other 10 bits accomodated by reserved and PAT bits)

	/*	program space is at 128 MB (0x8000000), in other words in index 32 of the directory_entry_array, so
	*	set the entry's present and read/write bits to 1 and page size to 1 since program memory
	*	is of size 4 MB
	*/
	directory_entry_array[PROGRAM_INDEX].present = 0;
	directory_entry_array[PROGRAM_INDEX].read_write = 1;
	directory_entry_array[PROGRAM_INDEX].page_size = 1;
	directory_entry_array[PROGRAM_INDEX].p_table_addr = SHELL_ADDR_1>>SHIFT_12;

	/* enable paging through assembly linkage */
	enable_paging(directory_entry_array);
}

/*
 * remap_page
 *   DESCRIPTION: Remaps and reinitializes paging (primarily for remapping program/shell address)
 *   INPUTS: pid - program id
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: remaps and reinitializes paging
 */
void remap_page(uint8_t pid) {

	directory_entry_array[PROGRAM_INDEX].present = 1;
	directory_entry_array[PROGRAM_INDEX].read_write = 1;
	directory_entry_array[PROGRAM_INDEX].page_size = 1;
	directory_entry_array[PROGRAM_INDEX].user_super = 1;
	directory_entry_array[PROGRAM_INDEX].p_table_addr = (SHELL_ADDR_1+pid*PROGRAM_SIZE)>>SHIFT_12;
	
	enable_paging(directory_entry_array);
}

/*
 * vid_page
 *   DESCRIPTION: Maps and initializes vidmap paging (for the currently displayed terminal)
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: maps and initializes vidmap paging
 */
void vid_page() {

	/*	vidmap virtual address will be at 136 MB, in other words in index 34 of the directory_entry_array, so
	*	set the entry's present and read/write bits to 1 and page size to 0 since video memory
	*	is of size 4 kB
	*/
	directory_entry_array[VID_MAP_INDEX].present = 1;
	directory_entry_array[VID_MAP_INDEX].read_write = 1;
	directory_entry_array[VID_MAP_INDEX].page_size = 0;
	directory_entry_array[VID_MAP_INDEX].user_super = 1;
	directory_entry_array[VID_MAP_INDEX].p_table_addr = ((int)vidmap_table_entry_array)>>SHIFT_12;

	/*	vidmap memory is at index 0 of the vid_table_entry_array, at 136 MB, so
	*	set the entry's present and read/write bits to 1 and set its base_address to the physical video address
	*/
	vidmap_table_entry_array[0].present = 1;
	vidmap_table_entry_array[0].read_write = 1;
	vidmap_table_entry_array[0].user_super = 1;
	vidmap_table_entry_array[0].p_base_addr = VIDEO_ADDR/KB_4;

	enable_paging(directory_entry_array);
	//uint32_t* pointer = VID_MAP_ADDR;
	//*pointer = 0;
}

/*
 * remap_real
 *   DESCRIPTION: Remaps and reinitializes vidmap paging for the currently displayed terminal in PIT handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: remaps and reinitializes vidmap paging to physical video memory
 */
void remap_real()
{
	// remap the virtual vidmap address to the physical video memory
	vidmap_table_entry_array[0].p_base_addr = VIDEO_ADDR/KB_4;	
	enable_paging(directory_entry_array);
}

/*
 * remap_shadow
 *   DESCRIPTION: Remaps and reinitializes vidmap paging for a non-displayed terminal in PIT handler
 *   INPUTS: uint32_t terminal - terminal to remap vidmap to
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: remaps and reinitializes vidmap paging
 */
void remap_shadow(uint32_t terminal)
{
	// based off of PIT's current terminal, remap the virtual vidmap address to the respective terminal buffer
	switch(terminal)
	{
		case TERM_1:
			vidmap_table_entry_array[0].p_base_addr = TERM_VID_1/KB_4;

			break;	
		case TERM_2:
			vidmap_table_entry_array[0].p_base_addr = TERM_VID_2/KB_4;

			break;
		case TERM_3:
			vidmap_table_entry_array[0].p_base_addr = TERM_VID_3/KB_4;

			break;
		default:
			return;break;
	}
	
	enable_paging(directory_entry_array);

}

/*
 * reset_mapping
 *   DESCRIPTION: resets mapping so that virtual terminal addresses point to their physical terminal buffers
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: resets terminal paging
 */
void reset_mapping()
{
	/*	terminal 1 memory is at 0xB9000, in other words in index TERM_VID_1/KB_4 of the table_entry_array, so
	*	set the entry's present and re/write bits to 1
	*/
	table_entry_array[TERM_VID_1/KB_4].present = 1;
	table_entry_array[TERM_VID_1/KB_4].read_write = 1;
	table_entry_array[TERM_VID_1/KB_4].p_base_addr = TERM_VID_1/KB_4;			// address has to be mapped from 4 kB
	
	/*	terminal 2 memory is at 0xBA000, in other words in index TERM_VID_2/KB_4 of the table_entry_array, so
	*	set the entry's present and re/write bits to 1
	*/
	table_entry_array[TERM_VID_2/KB_4].present = 1;
	table_entry_array[TERM_VID_2/KB_4].read_write = 1;
	table_entry_array[TERM_VID_2/KB_4].p_base_addr = TERM_VID_2/KB_4;			// address has to be mapped from 4 kB
	
	/*	terminal 3 memory is at 0xBB000, in other words in index TERM_VID_3/KB_4 of the table_entry_array, so
	*	set the entry's present and re/write bits to 1
	*/
	table_entry_array[TERM_VID_3/KB_4].present = 1;
	table_entry_array[TERM_VID_3/KB_4].read_write = 1;
	table_entry_array[TERM_VID_3/KB_4].p_base_addr = TERM_VID_3/KB_4;			// address has to be mapped from 4 kB

	enable_paging(directory_entry_array);
}

/*
 * map_terminal
 *   DESCRIPTION: maps terminals so that virtual address of currently displayed temrinal maps to physical video memory
 *   INPUTS: uint32_t terminal - terminal that's to be displayed
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: sets the virtual address of the displayed terminal to physical video memory
 */
void map_terminal(uint32_t terminal)
{
	// based off of the currently displayed terminal, remap the virtual terminal address to the physical video memory
	switch(terminal)
	{
		case TERM_1:
			table_entry_array[TERM_VID_1/KB_4].p_base_addr = VIDEO_ADDR/KB_4;
			break;	
		case TERM_2:
			table_entry_array[TERM_VID_2/KB_4].p_base_addr = VIDEO_ADDR/KB_4;
			break;
		case TERM_3:
			table_entry_array[TERM_VID_3/KB_4].p_base_addr = VIDEO_ADDR/KB_4;
			break;
		default:
			return;
	}

	enable_paging(directory_entry_array);
}
