#include<linux/module.h>
#include<linux/string.h>
#include<linux/fs.h>
#include<asm/uaccess.h>
#include<asm/errno.h>
#include<linux/kernel.h>
#include<linux/cdev.h>
#include<linux/init.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Char Device");
MODULE_AUTHOR("Gelivi");

/*
* creates and returns an inode based on the parmeters
*/
struct inode *tagfs_get_inode(struct super_block *sb, const struct inode *dir, umode_t mode, dev_t dev){
	/* Allocates a new inode for given superblock. */
	struct inode *inode = new_inode(sb);

	if (inode) {
		inode->i_ino = get_next_ino();
		
		/* inode_init_owner - Init uid,gid,mode for new inode according to posix standards */
		inode_init_owner(inode, dir, mode);

		inode->i_atime = inode->i_mtime = inode->i_ctime = CURRENT_TIME;

		if((mode & S_IFMT) == S_IFDIR) {
			/* i_nlink -number of hard links - will be initialized to 1 in the inode_init_always function
			 * (that gets called inside the new_inode function),
			 * We change it to 2 for directories, for covering the "." entry */
			inc_nlink(inode);
		}
	}
	return inode;
}

/*	method to read in superblock
 *	sb - The VFS superblock
 * 	data - Mount options
 * 	silent - Don't complain 
 */
int tagfs_fill_super(struct super_block * sb, void * data, int silent){
	struct inode * inode;
	
	/* A number to uniquely identify our file system
	 * for more go to http://en.wikipedia.org/wiki/File_format#Magic_number
	 */
	sb->s_magic=0x14031992;
	inode = tagfs_get_inode(sb, NULL, S_IFDIR, 0);
	sb->s_root = d_make_root(inode);
	if(!sb->s_root) return -ENOMEM;

	return 0;
	
}

static struct dentry * tagfs_mount(struct file_system_type * fs_type, int flags, const char * dev_name, void *data){
	struct dentry *ret;
	ret = mount_bdev(fs_type, flags, dev_name, data, tagfs_fill_super);
	/* unlikely - macro that give hints to the compiler about which way a branch may go. */
	if (unlikely(IS_ERR(ret)))
		printk(KERN_ERR "Error mounting tagfs");
	else
		printk(KERN_INFO "tagfs is succesfully mounted on [%s]\n", dev_name);

	return ret;
}

static void tagfs_kill_superblock(struct super_block *s){
	printk(KERN_INFO "tagfs superblock is destroyed. Unmount succesful.\n");
	return;
}

struct file_system_type tagfs_fs_type = {
	.owner = THIS_MODULE,
	.name = "tagfs",
	.mount = tagfs_mount,
	.kill_sb = tagfs_kill_superblock,
};

static int tagfs_init(void){
	int ret;
	ret = register_filesystem(&tagfs_fs_type);
	if (likely(ret == 0))
		printk(KERN_INFO "Sucessfully registered tagfs\n");
	else
		printk(KERN_ERR "Failed to register tagfs. Error:[%d]", ret);
	return ret;
}

static void tagfs_exit(void){
	int ret;
	ret = unregister_filesystem(&tagfs_fs_type);
	if (likely(ret == 0))
		printk(KERN_INFO "Sucessfully unregistered tagfs\n");
	else
		printk(KERN_ERR "Failed to unregister tagfs. Error:[%d]", ret);
}

module_init(tagfs_init);
module_exit(tagfs_exit);
