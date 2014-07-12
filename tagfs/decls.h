#define TFS_ROOT_INO	(2)


/* maximum number of blocks a file can contains */
#define TFS_N_BLOCKS	(4)
#define TFS_BLOCK_SIZE	(4096)
#define TFS_ROOT_BLOCK	(8)
#define TFS_SUPER_BLOCK	(1)
#define TFS_INODE_TABLE_BLOCK	(3)

/*file name maximum length*/
#define TFS_FILE_MAXLEN	(64)

#define TFS_MAGIC	(0x14031992)
#define tfs_debug(f, ...)						\
		printk(KERN_DEBUG "[gelivi tagfs] DEBUG (%s, %d): %s:",	\
			__FILE__, __LINE__, __func__);			\
		printk(KERN_DEBUG f, ## __VA_ARGS__);
typedef uint32_t tint;

/*on disk DS*/
struct tfs_inode{
	tint i_mode;
	tint i_ino;
	union{
		tint i_size;
		tint i_child_count;
	};	
	tint i_atime;
	tint i_ctime;
	tint i_mtime;
	tint i_uid;
	tint i_gid;
	tint i_flags;
	tint i_links_count;
	tint i_blocks;			/*blocks count*/
	tint i_block[TFS_N_BLOCKS];		/*pointers to blocks*/
};
/*on disk DS*/
struct tfs_super_block{
	tint s_inodes_count;
	tint s_blocks_count;
	tint s_free_inodes_count;
	tint s_free_blocks_count;
	tint s_magic;
	tint s_first_data_block;
	tint s_first_ino;
	tint s_inode_size;
	tint s_r_blocks_count;
	tint s_state;
	tint s_block_size;
	char padding[TFS_BLOCK_SIZE - (10*sizeof(tint))];
};

/*on disk DS -- a directory file is filled with these structures*/
struct tfs_dir_entry{
	tint ino;
	tint entry_len;
	tint name_len;
	tint file_type;
	char name[TFS_FILE_MAXLEN];
};














