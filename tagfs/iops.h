struct dentry * tfs_iops_lookup (struct inode * dir, struct dentry *dentry, unsigned int flags){
	static int dbug = 0;
	struct inode * inode;
	ino_t ino=0;
	struct buffer_head *bh;
	int i,children;
	const char * name = dentry->d_name.name;
	struct tfs_dir_entry *entry;
	struct super_block *sb = dir->i_sb;
	struct tfs_inode_mem *inode_mem = dir->i_private;
	children = inode_mem->i_child_count;
	bh = sb_bread(sb, inode_mem->i_block[0]);
	entry = (struct tfs_dir_entry *)bh->b_data;
	for(i=0; i<children; i++){
		printk(KERN_INFO "[ dbug %d ]in lookup name - %s and inode numbr - %d\n", dbug++, entry->name, entry->ino);
		if(!strcmp(name, entry->name)){
			ino = entry->ino;
			break;
		}
		entry++;
	}
	if(ino){
		inode = tfs_iget(sb, ino);
	}else{
		printk(KERN_ERR"couldnt find the file %s\n",name);
		return NULL;
	}
	return d_splice_alias(inode, dentry);
}

