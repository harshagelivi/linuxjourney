/* fstype_fun.h */
struct dentry * tagfs_mount(struct file_system_type * tfs_type, int flags, const char * name, void * data);
void tagfs_kill_sb(struct super_block * sb);
/*	tagfs_fill_super - method to read in superblock */
int tagfs_fill_super(struct super_block * sb, void * data, int); 

/* fops.h */
static int tfs_dirops_iterate (struct file *, struct dir_context *);
int tfs_fops_open(struct inode *, struct file *);
ssize_t tfs_fops_read(struct file *, char __user *, size_t, loff_t *);

/* iops.h */
struct dentry * tfs_iops_lookup (struct inode *,struct dentry *, unsigned int);
struct inode * tfs_iget(struct super_block * sb, unsigned int ino);
static struct tfs_inode * tfs_get_inode(struct super_block * sb, unsigned int ino, struct buffer_head **p);

