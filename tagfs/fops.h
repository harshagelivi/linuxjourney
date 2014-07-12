static int tfs_dirops_iterate (struct file * filp, struct dir_context * dcxt){
	loff_t pos = dcxt->pos;
	struct inode *inode = file_inode(filp);
	struct super_block *sb = inode->i_sb;
	struct buffer_head *bh;
	struct tfs_dir_entry *entry;
	struct tfs_inode_mem *inode_mem;
	int children, i;
	if(pos) return 0;
	inode_mem = inode->i_private;
	if(!S_ISDIR(inode->i_mode)){
		printk(KERN_ERR"not a directry\n");
		return -ENOTDIR;
	}
	/*FIXME takes only one block as of now*/
	if(! (bh = sb_bread(sb, inode_mem->i_block[0])) ){
		printk(KERN_ERR "Cannot read data block\n");	
		
	};
	entry = (struct tfs_dir_entry *)bh->b_data;
	children = inode_mem->i_child_count;
	for(i=0;i<children;i++){
		dir_emit(dcxt, entry->name, TFS_FILE_MAXLEN, entry->ino, DT_UNKNOWN);
		dcxt->pos += sizeof(struct tfs_dir_entry);
		entry++;
	}
	brelse(bh);
	printk(KERN_INFO " in iterate pos %d i_size %d sbcount %d\n", (int)pos, (int)inode->i_size, (int)sb->s_count);
	return 0;
}
