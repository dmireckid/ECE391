/* paging_assem.h - Enable paging
 */

#ifndef _PAGING_ASSEM
#define _PAGING_ASSEM

#include "paging.h"

// enable paging by properly setting the control register values
extern void enable_paging(page_directory_entry_t* pointer);

#endif
