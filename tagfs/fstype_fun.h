struct dentry * tagfs_mount(struct file_system_type * tfs_type, int flags, const char * name, void * data){
	struct dentry * ret;
	printk(KERN_INFO "Mounted with flags %d and name %s\n", flags, name);
	ret = mount_bdev(tfs_type, flags, name, data, tagfs_fill_super);
	if( unlikely(IS_ERR(ret)) ){
		tfs_debug("failed in mount\n");
	}else{
		
	}
	return ret;
}
void tagfs_kill_sb(struct super_block * sb){
	kill_block_super(sb);
	printk(KERN_INFO"[tagfs gelivi] in kill sb\n");
}
int tagfs_fill_super(struct super_block * sb, void * data, int silent){
	struct inode * root_inode;
	struct tfs_super_block * tsb;
	int ret;
	ret = -ENOMEM;
	tsb = kzalloc(sizeof(*tsb), GFP_KERNEL);
	if(!tsb){
		goto failed;
	}
	root_inode = new_inode(sb);
	inode_init_owner(root_inode, NULL, S_IFDIR);
	root_inode->i_sb = sb;
	root_inode->i_ino = TFS_ROOT_INO;
	root_inode->i_op = &tfs_iops;
	root_inode->i_fop = &tfs_fops;
	sb->s_fs_info = tsb;
	sb->s_type = &tfs_type;
	sb->s_magic = TFS_MAGIC;
	sb->s_root = d_make_root(root_inode);
	if(!sb->s_root){
		printk(KERN_INFO "failed in fill super\n");
		return -ENOMEM;
	}
	ret = 0;
failed:	
	return ret;	
}
