const unsigned int TAGFS_MAGIC_NUMBER = 0x14031992;
const unsigned int TAGFS_DEFAULT_BLOCK_SIZE = 1024 * 4;

struct tagfs_super_block {
	unsigned int version;
	unsigned int magic;
	unsigned int block_size;
	unsigned int free_blocks;
	char padding[ (4*1024) - (4 * sizeof(unsigned int))];
};
