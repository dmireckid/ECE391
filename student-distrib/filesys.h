//FILESYS_PARSE HEADER FILE
#include "types.h"
#include "lib.h"

#define FILENAME_LEN 32

typedef struct{
  int8_t filename[FILENAME_LEN];
  int32_t filetype;
  int32_t inode_num;
  int8_t reserved[24];
}dentry_t;    //directory entry struct

typedef struct{
  int32_t i_length;
  int32_t data_block_num[1023];
}inode_t;   //index node struct


typedef struct{
  int32_t dir_count;
  int32_t inode_count;
  int32_t data_count;
  int8_t reserved[52];
  dentry_t direntries[63];
}bootblock_t;   //bootblock struct

void init_filesystem(uint32_t address);
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
int32_t open_f(const uint8_t* fname);
int32_t open_d(const uint8_t* fname);
int32_t close_f(int32_t fd);
int32_t close_d(int32_t fd);
int32_t read_f(int32_t fd, uint32_t offset, void* buf,uint32_t length);
int32_t read_d(int32_t fp, uint8_t* buf, int32_t count);
int32_t write_f(int32_t fd, int32_t count, void* buf);
int32_t write_d(int32_t fd, int32_t count, void* buf);

