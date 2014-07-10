struct dentry * tagfs_mount(struct file_system_type * tfs_type, int flags, const char * name, void * data);
void tagfs_kill_sb(struct super_block * sb);
int tagfs_fill_super(struct super_block * sb, void * data, int); 
