int tfs_dirops_iterate (struct file * filp, struct dir_context * dcxt){
	loff_t pos = dcxt->pos;
	struct inode * inode = file_inode(filp);
	struct super_block * sb = inode->i_sb;
	
	printk(KERN_INFO " in iterate pos %d i_size %d sbcount %d\n", (int)pos, (int)inode->i_size, (int)sb->s_count);
	return 0;
}
