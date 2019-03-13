//FILESYS_PARSE HEADER FILE

int32 t read dentry by name (const uint8 t* fname, dentry t* dentry);
int32 t read dentry by index (uint32 t index, dentry t* dentry);
int32 t read data (uint32 t inode, uint32 t offset, uint8 t* buf, uint32 t length);