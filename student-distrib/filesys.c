#include "filesys.h"
#include "types.h"
#include "lib.h"

static uint32_t location_fs, location_i, location_d;
static uint32_t num_direntries, num_inodes, num_datablocks, num_reads;
bootblock_t* super_block;


/*
 * init_filesystem
 *   DESCRIPTION: Initializes the filesystem to proper values.
 *   INPUTS: fs_addr
 *   OUTPUTS: NONE
 *   RETURN VALUE: NONE
 *   SIDE EFFECTS: bootblock struct will be
 *								 initialized
 */
void init_filesystem(uint32_t fs_addr){
	uint32_t size = 4096;	//4kB, size of data block
	location_fs = fs_addr;
	location_i = fs_addr + size;
	super_block = (bootblock_t*)fs_addr;

	num_direntries = super_block->dir_count;
	num_inodes = super_block->inode_count;
	num_datablocks = super_block->data_count;
	num_reads = 0;
	location_d = location_i + size*num_inodes;
}

/*
 * read_dentry_by_name
 *   DESCRIPTION: Reads a directory entry by the name of a file
 *   INPUTS: const uint8_t* fname, dentry_t* dentry
 *   OUTPUTS: int32_t
 *   RETURN VALUE: 0 for success, -1 for failure
 *   SIDE EFFECTS: Finds the filename in the bootblock,
 *								 and updates the inputted dentry with
 *								 the file's values
 */
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){
	uint32_t i;
	for(i=0; i<num_direntries; i++){		//loop thru all dentries in bootblock until filename is found
		uint8_t* direntry = (uint8_t*)super_block->direntries[i].filename;
		if(strncmp((int8_t*)fname, (int8_t*)direntry, FILENAME_LEN) == 0){		//filename found
			read_dentry_by_index(i, dentry);		//use found file's index to update dentry
			return 0;
		}
	}
	return -1;
}

/*
 * read_dentry_by_index
 *   DESCRIPTION: Reads a directory entry by the index of a file
 *   INPUTS:  uint8_t index, dentry_t* dentry
 *   OUTPUTS: int32_t
 *   RETURN VALUE: 0 for success, -1 for failure
 *   SIDE EFFECTS: Goes to the given index in the bootblock,
 *								 and updates the inputted dentry with
 *								 the file's values
 */
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry){
	if(index > num_direntries-1){		//index out of range
		return -1;
	}
	else{
		strncpy((int8_t*)dentry->filename, (int8_t*)super_block->direntries[index].filename, FILENAME_LEN);
		dentry->filetype = super_block->direntries[index].filetype;	//update dentry values
		dentry->inode_num = super_block->direntries[index].inode_num;
		return 0;
	}
}

/*
 * read_data
 *   DESCRIPTION: Reads a file's contents
 *   INPUTS:  uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length
 *   OUTPUTS: int32_t retval
 *   RETURN VALUE: returns nunmber of bytes in the file, -1 for failure
 *   SIDE EFFECTS: Updates the buffer with the file's contents
 */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
	//printf("Got to read data\n");
	uint32_t i, start, end;
	uint32_t size = 4096;	//4kB size of a block
	uint32_t retval = 0;
	start = offset / size;		//set start and end points for loop
	end = (offset+length)/size;

	uint32_t n, location_b;
	uint8_t* location_r;
	//uint32_t temp = ;
	//uint32_t temp2;
	inode_t* inode_to_read = (inode_t*)(location_i + size*inode);

	if(inode > num_inodes-1){
		printf("problem 1\n");	//inode value out of range
		return -1;
	}
	if(offset >= inode_to_read->i_length){		//offset out of range
		return 0;
	}
	if(offset+length >= (inode_to_read->i_length)){	//shorten length if out of range
		length = (inode_to_read->i_length) - offset;
	}
	//printf("Pre-forloop\n");
	//printf("%d\n", num_datablocks);
	for (i= start ; i<= end ; i++)
	{
		if (inode_to_read->data_block_num[i] > num_datablocks-1){
			return -1;		//failure if data block num is greater than number of total datablocks
		}
		else{
			location_b = inode_to_read->data_block_num[i];
		}
		if (((offset%size) + length) >= size){ //check if out of block size
			n = size - (offset%size);						 //set reading length to within bounds
		}
		else{
			n = length;
		}
		location_r = (uint8_t*)(location_d + location_b*size + (offset%size));
		memcpy(buf, location_r, n); //copy data to the buffer
		buf += n;
		offset += n;		//update values
		length -= n;
		retval += n;
	}
	return retval;		//return number of bytes in the file
}

/*
 * open_f
 *   DESCRIPTION: "open" a file"
 *   INPUTS:  const uint8_t* fname (name of file to open)
 *   OUTPUTS: int32_t
 *   RETURN VALUE: 0 for success, -1 for failure
 *   SIDE EFFECTS: NONE
 */
int32_t open_f(const uint8_t* fname){
	//initialize temp structs, then return 0

	return 0;
}

/*
 * open_d
 *   DESCRIPTION: "open" a directory
 *   INPUTS:  const uint8_t* fname (name of dir to open)
 *   OUTPUTS: int32_t
 *   RETURN VALUE: 0 for success, -1 for failure
 *   SIDE EFFECTS: NONE
 */
int32_t open_d(const uint8_t* fname){
	//read_dentry_by_name
	return 0;
}

/*
 * close_f
 *   DESCRIPTION: "close" a file
 *   INPUTS:  int8_t fd (descriptor of file to close)
 *   OUTPUTS: int32_t
 *   RETURN VALUE: 0 for success, -1 for failure
 *   SIDE EFFECTS: NONE
 */
int32_t close_f(int32_t fd){
	//undo temp structs from open_f, then return success
	return 0;
}

/*
 * close_d
 *   DESCRIPTION: "close" a directory
 *   INPUTS:  int8_t fd (descriptor of dir to close)
 *   OUTPUTS: int32_t
 *   RETURN VALUE: 0 for success, -1 for failure
 *   SIDE EFFECTS: NONE
 */
int32_t close_d(int32_t fd){
	return 0;				//do nothing, "successfully" closed
}

/*
 * read_f
 *   DESCRIPTION: read the contents of a file
 *   INPUTS:  int32_t fd, uint32_t offset, void* buf,uint32_t length
 *   OUTPUTS: int32_t
 *   RETURN VALUE: length of file, -1 for failure
 *   SIDE EFFECTS: fills buffer with contents of the file
 */
int32_t read_f(int32_t fd, uint32_t offset, void* buf,uint32_t length){
	dentry_t* direntry;
	if(read_dentry_by_name((uint8_t*)fd, direntry) == -1){
		return -1;
	}
	return read_data(direntry->inode_num, offset, buf, length);
}

/*
 * read_d
 *   DESCRIPTION: read the contents of a directory
 *   INPUTS:  int32_t fd, int32_t count, uint8_t* buf
 *   OUTPUTS: int32_t
 *   RETURN VALUE: length of buffer, -1 for failure
 *   SIDE EFFECTS: fills buffer with contents of directory
 */
int32_t read_d(int32_t fd, int32_t count, uint8_t* buf){
	if(num_reads > num_direntries-1){
		num_reads = 0;
		return 0;
	}
	strcpy((int8_t*)buf, (const int8_t*)super_block->direntries[num_reads].filename);
	num_reads++;
	return strlen((int8_t*)buf);
}

/*
 * write_f
 *   DESCRIPTION: write to a file (read-only!)
 *   INPUTS:  int32_t fd, int32_t count, void* buf
 *   OUTPUTS: int32_t
 *   RETURN VALUE: 0 for success, -1 for failure
 *   SIDE EFFECTS: NONE
 */
int32_t write_f(int32_t fd, int32_t count, void* buf){
	return -1;				//read-only filesystem, cannot write
}

/*
 * write_d
 *   DESCRIPTION: write to a directory (read-only!)
 *   INPUTS:  int32_t fd, int32_t count, void* buf
 *   OUTPUTS: int32_t
 *   RETURN VALUE: 0 for success, -1 for failure
 *   SIDE EFFECTS: NONE
 */
int32_t write_d(int32_t fd, int32_t count, void* buf){
	return -1;				//read-only filesystem, cannot write
}

