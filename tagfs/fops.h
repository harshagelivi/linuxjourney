static int tfs_dirops_iterate (struct file * filp, struct dir_context * dcxt){
	static int callno=0;
	loff_t pos = dcxt->pos;
	struct inode *inode = file_inode(filp);
	struct super_block *sb = inode->i_sb;
	struct buffer_head *bh;
	struct tfs_dir_entry *entry;
	struct tfs_inode_mem *inode_mem;
	int children, i, block;
	if(pos) return 0;
	inode_mem = inode->i_private;
	if(!S_ISDIR(inode->i_mode)){
		printk(KERN_ERR"not a directry\n");
		return -ENOTDIR;
	}
	/*FIXME takes only one block as of now*/
	block = inode_mem->i_block[0];
	if(! (bh = sb_bread(sb, block)) ){
		printk(KERN_ERR "Cannot read data block\n");	
		
	};
	entry = (struct tfs_dir_entry *)bh->b_data;
	children = inode_mem->i_child_count;
	for(i=0;i<children;i++){
		dir_emit(dcxt, entry->name, TFS_FILE_MAXLEN, entry->ino, DT_UNKNOWN);
		dcxt->pos += sizeof(struct tfs_dir_entry);
		printk(KERN_INFO "[global - %d, local - %d] in iterate for name - %s pos %d i_size %d children count %d and block number - %d\n",glo_callno++, callno, entry->name, (int)dcxt->pos, (int)inode->i_size, (int)children, block);
		entry++;
	}
	brelse(bh);

	return 0;
}
int tfs_fops_open(struct inode *inode, struct file *filp){
	printk(KERN_INFO "[global - %d] in fops open for file inode -%d with offset %d\n", glo_callno++, (int)(inode->i_ino), (int)(filp->f_pos));
	filp->f_ra.start = 0;
	return generic_file_open(inode, filp);
}
ssize_t tfs_fops_read(struct file *filp, char __user *buff, size_t sz, loff_t * offset){
	int block, len, fsz;
	char * data;
	struct inode *inode = filp->f_inode;
	struct super_block *sb = inode->i_sb;
	struct tfs_inode_mem *tfs_inode_mem = inode->i_private;
	struct buffer_head * bh;
	
	fsz = inode->i_size;
	if(*offset > fsz){
		printk("asked for offset greater than file size");
		return 0;
	}
	/*FIXME get block number based on offset*/
	block = tfs_inode_mem->i_block[0]; 
	bh = sb_bread(sb, block);
	if(!bh){
		return -1;
	}
	data = (bh->b_data + *offset);
	len = fsz > sz ? sz : fsz;
	if (copy_to_user(buff, data, len)) {
		brelse(bh);
		printk("Error: cant copy file data to user space\n");
		return -EFAULT;
	}
	printk("data is %s\n", data);
	brelse(bh);	
	*offset += len;
	printk(KERN_INFO "[global - %d] in fops read for file with f_pos %d flags %d mode %d and file size %d of size_t %d with offset %d and len is %d\n",
			glo_callno++, (int)(filp->f_pos), (int)(filp->f_flags), (int)(filp->f_mode), fsz, (int)sz, (int)(*offset), len);
	return len;		
}
