#ifndef _PAGING
#define _PAGING

#define 4_KB	4096
#define 4_MB	4194304

#define KERNEL_ADDR		0x400000
#define VIDEO_ADDR		0xB8000

#define NUM_ENTRIES		1024

/* struct for page directory entry */
typedef struct page_directory_entry{
	union {
		uint32_t val;
		struct {
			uint32_t present		: 1;	// set to 0 or 1
			uint32_t read_write		: 1;	// set to 0
			uint32_t user_super		: 1;	// set to 0
			uint32_t write_through	: 1;	// set to 0
			uint32_t cache_disabled	: 1;	// set to 0
			uint32_t accessed		: 1;	// set to 0
			uint32_t reserved		: 1;	// set to 0
			uint32_t page_size		: 1;	// set to 0 for 4 kB
			uint32_t global_page	: 1;	// set to 0
			uint32_t available		: 3;	// set to 0
			uint32_t p_table_addr	: 20;	// page table address
	    } __attribute__ ((packed));
    };
} page_directory_entry_t;

/* struct for page table entry */
typedef struct page_table_entry{
	union {
		uint32_t val;
		struct {
			uint32_t present		: 1;
			uint32_t read_write		: 1;
			uint32_t user_super		: 1;
			uint32_t write_through	: 1;
			uint32_t cache_disabled	: 1;
			uint32_t accessed		: 1;
			uint32_t dirty			: 1;
			uint32_t page_attribute	: 1;	// page table attribute index
			uint32_t global_page	: 1;
			uint32_t available		: 3;
			uint32_t p_base_addr	: 20;
		} __attribute__ ((packed));
    };
} page_table_entry_t;


/* global arrays for directory and table entries */
page_directory_entry_t directory_entry_array[NUM_ENTRIES] __attribute__((aligned (4_KB)));
page_table_entry_t	table_entry_array[NUM_ENTRIES] __attribute__((aligned (4_KB)));


/* function to initialize paging */
extern void initialize_page();