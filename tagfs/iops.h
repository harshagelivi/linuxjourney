struct dentry * tfs_iops_lookup (struct inode * dir, struct dentry *dentry, unsigned int flags){
	static int callno = 0;
	struct inode * inode;
	ino_t ino=0;
	struct buffer_head *bh;
	int i,children, block;
	const char * name = dentry->d_name.name;
	struct tfs_dir_entry *entry;
	struct super_block *sb = dir->i_sb;
	struct tfs_inode_mem *inode_mem = dir->i_private;
	
	children = inode_mem->i_child_count;
	block = inode_mem->i_block[0];
	bh = sb_bread(sb, block);
	entry = (struct tfs_dir_entry *)bh->b_data;
	
	for(i=0; i<children; i++){
		printk(KERN_INFO "[ global - %d local - %d ]in lookup name - %s and inode numbr - %d and children count - %d and block number - %d\n", glo_callno++, callno++, entry->name, entry->ino, children, block);
		if(!strcmp(name, entry->name)){
			ino = entry->ino;
			break;
		}
		entry++;
	}
	if(ino){
		inode = tfs_iget(sb, ino);
		if(!inode){
			printk(KERN_ERR"couldnt fetch inode for the file %s\n",name);	
		}
		d_add(dentry, inode);
	}else{
		printk(KERN_ERR"couldnt find the file %s\n",name);
	}
	return NULL;
}
struct inode * tfs_iget(struct super_block * sb, unsigned int ino){
	struct inode *inode;
	struct tfs_inode *inode_disk;
	struct buffer_head *bh;
	struct tfs_inode_mem *tfs_inode_mem;
	int blocks,i;

	tfs_inode_mem = kzalloc(sizeof(struct tfs_inode_mem), GFP_KERNEL);
	if(!tfs_inode_mem){
		return ERR_PTR(-ENOMEM);
	}
	
	inode = iget_locked(sb, ino);
	if(!inode){
		return ERR_PTR(-ENOMEM);
	}
	if(!(inode->i_state & I_NEW)){
		return inode;
	}

	inode_disk = tfs_get_inode(sb, ino, &bh);
	if(IS_ERR(inode_disk)){
		iget_failed(inode);
		return ERR_PTR(-EIO);
	}
	inode->i_mode = inode_disk->i_mode;
	i_uid_write(inode, inode_disk->i_uid);
	i_gid_write(inode, inode_disk->i_gid);

	set_nlink(inode, inode_disk->i_links_count);
	inode->i_atime.tv_sec = (signed)inode_disk->i_atime;
	inode->i_ctime.tv_sec = (signed)inode_disk->i_ctime;
	inode->i_mtime.tv_sec = (signed)inode_disk->i_mtime;
	inode->i_atime.tv_nsec = inode->i_mtime.tv_nsec = inode->i_ctime.tv_nsec = 0;

	blocks = tfs_inode_mem->i_blocks = inode->i_blocks = inode_disk->i_blocks;
	for(i=0; i<blocks; i++){
		tfs_inode_mem->i_block[i] = inode_disk->i_block[i];
	}
	inode->i_private = tfs_inode_mem;
	if(S_ISREG(inode->i_mode)){
		tfs_inode_mem->i_size = inode->i_size = inode_disk->i_size;
		inode->i_op = &tfs_iops;
		inode->i_fop = &tfs_fops;
	}else if(S_ISDIR(inode->i_mode)){
		tfs_inode_mem->i_child_count = inode_disk->i_child_count;
		inode->i_op = &tfs_iops;
		inode->i_fop = &tfs_dirops;

	}
	inode->i_flags = inode_disk->i_flags;
	brelse (bh);
	unlock_new_inode(inode);
	return inode;	
}
static struct tfs_inode * tfs_get_inode(struct super_block * sb, unsigned int ino, struct buffer_head **p){
	struct buffer_head * bh;
	unsigned int offset;
	offset = sizeof(struct tfs_inode)*ino;
	/*FIXME not the correct block always*/
	if(! (bh = sb_bread(sb, TFS_INODE_TABLE_BLOCK)) ){
		return ERR_PTR(-EIO);
	}
	*p=bh;
	return (struct tfs_inode *)(bh->b_data+offset);
}
