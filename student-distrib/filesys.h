//FILESYS_PARSE HEADER FILE

int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
int32_t open_f(const uint8_t* fname);
int32_t open_d(const uint8_t* fname);
int32_t close_f(int32_t f);
int32_t close_d(int32_t d);
int32_t read_f(int32_t f, int32_t count, void* buf);
int32_t read_d(int32_t d, int32_t count, void* buf);
int32_t write_f(int32_t f, int32_t count, void* buf);
int32_t write_d(int32_t d, int32_t count, void* buf);
