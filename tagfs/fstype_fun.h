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

/*	method to read in superblock
 *	sb - The VFS superblock
 *	data - Mount options
 *	silent - Don't complain 
 */
int tagfs_fill_super(struct super_block * sb, void * data, int silent){
	struct inode * root_inode;
	struct buffer_head *  bh;
	struct tfs_super_block * tsb_disk;
	struct tfs_super_block_mem * tsb_mem;
	int ret=0;
	unsigned int offset=0;
	tsb_mem = kzalloc(sizeof(*tsb_mem), GFP_KERNEL);
	if(!tsb_mem){
		ret = -ENOMEM;
		goto failed;
	}
	sb->s_fs_info = tsb_mem;
	spin_lock_init(&tsb_mem->s_lock);

	/*this sets the block size in sb*/
	if(sb_min_blocksize(sb, TFS_BLOCK_SIZE) == 0){
		printk(KERN_ERR "Cannot set blocksize\n");
		ret = -EPERM;
		goto failedsbmem;
	};
	/* sb_read returns a buffer head which contains 
 	 * given block of size sb->s_blocksize data from sb->s_bdev
 	 */

	if(! (bh = sb_bread(sb, TFS_SUPER_BLOCK)) ){
		printk(KERN_ERR "Cannot read super block\n");	
		ret = -EPERM;
		goto failedsbmem;
	};
	/* b_data - pointer to data within the page */
	offset = TFS_SUPER_BLOCK * TFS_BLOCK_SIZE;
	tsb_disk = (struct tfs_super_block *)(bh->b_data);
	printk(KERN_INFO "The magic number obtained in disk with offset %d and blocksize %d is: [%0x]\n", offset, tsb_disk->s_block_size, tsb_disk->s_magic);
	if (unlikely(tsb_disk->s_magic != TFS_MAGIC)) {
		ret = -EINVAL;
		goto failedmagic;
	}

	if (unlikely(tsb_disk->s_block_size != TFS_BLOCK_SIZE)) {
		printk(KERN_ERR "Bad block size.");
		ret = -EPERM;
		goto failedmagic;
	}
	tsb_mem->s_sb_bh = bh;
	sb->s_op = &tfs_sops;

	root_inode = tfs_iget(sb, TFS_ROOT_INO);
	if( unlikely(IS_ERR(root_inode)) ){
		printk(KERN_ERR "Failed in getting root inode\n");
		ret = PTR_ERR(root_inode);
		goto failedbsize;
	}

	if( unlikely(!S_ISDIR(root_inode->i_mode)) ){
		printk(KERN_ERR "Corrupt root inode\n");
		ret = -EINVAL;
		goto failedbsize;
	}
	/*
	root_inode = new_inode(sb);
	inode_init_owner(root_inode, NULL, S_IFDIR);
	root_inode->i_sb = sb;
	root_inode->i_ino = TFS_ROOT_INO;
	root_inode->i_op = &tfs_iops;
	root_inode->i_fop = &tfs_fops;
	*/
	sb->s_root = d_make_root(root_inode);
	if(!sb->s_root){
		printk(KERN_INFO "failed in fill super\n");
		ret = -ENOMEM;
		goto failedbsize;
	}
	return ret;
failedmagic:
	if(!silent){
		printk(KERN_ERR "Cannot find tagfs in the device");
	}
failedbsize:	
	brelse(bh);
failedsbmem:	
	sb->s_fs_info = NULL;	
	kfree(tsb_mem);
failed:	
	return ret;	
}
struct inode * tfs_iget(struct super_block * sb, unsigned int ino){
	struct inode *inode;
	struct tfs_inode *inode_disk;
	struct buffer_head *bh;
	struct tfs_inode_mem *tfs_inode_mem;
	int blocks,i;

	tfs_inode_mem = kzalloc(sizeof(struct tfs_inode_mem), GFP_KERNEL);
	
	inode = iget_locked(sb, ino);
	if(!ino){
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
