struct dentry * tagfs_mount(struct file_system_type * tfs_type, int flags, const char * name, void * data){
	struct dentry * ret;
	ret = mount_bdev(tfs_type, flags, name, data, tagfs_fill_super);
	if( unlikely(IS_ERR(ret)) ){
		printk(KERN_ERR "failed in mount\n");
	}else{
		printk(KERN_INFO "Mounted with flags %d and name %s\n", flags, name);		
	}
	return ret;
}
void tagfs_kill_sb(struct super_block * sb){
	kill_block_super(sb);
	printk(KERN_INFO"[tagfs gelivi] in kill sb\n");
}

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

