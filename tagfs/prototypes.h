struct dentry * tagfs_mount(struct file_system_type * tfs_type, int flags, const char * name, void * data);
void tagfs_kill_sb(struct super_block * sb);
int tagfs_fill_super(struct super_block * sb, void * data, int); 
int tfs_dirops_iterate (struct file *, struct dir_context *);
struct dentry * tfs_iops_lookup (struct inode *,struct dentry *, unsigned int);
struct inode * tfs_iget(struct super_block * sb, unsigned int ino);
static struct tfs_inode * tfs_get_inode(struct super_block * sb, unsigned int ino, struct buffer_head **p);


