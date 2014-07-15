#include <linux/module.h>
#include <linux/init.h>
#include<linux/kernel.h>
#include<linux/slab.h>
#include<linux/fs.h>
#include<linux/uaccess.h>
#include<linux/errno.h>
#include<linux/cdev.h>
#include<linux/string.h>
#include<linux/buffer_head.h>
#include "decls.h"

static int glo_callno = 0;

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A Simple File System");
MODULE_AUTHOR("Gelivi Harsha Vardhan");

/*super block for tagfs in memory*/
struct tfs_super_block_mem{
	struct buffer_head * s_sb_bh;	/* Buffer containing the super block */
	struct tfs_super_block * s_tsb;	/* Pointer to the super block in the buffer */
	spinlock_t s_lock;

};

/*disk inode related information in memory*/
struct tfs_inode_mem{
	tint i_blocks;
	union{
		tint i_size;
		tint i_child_count;
	};
	tint i_block[TFS_N_BLOCKS];
};

#include "prototypes.h"

static struct file_system_type tfs_type={
	.owner	=	THIS_MODULE,
	.name	=	"tagfs",
	.mount	=	tagfs_mount,
	.kill_sb	=	tagfs_kill_sb,
};
static struct super_operations tfs_sops = {

};
static struct inode_operations tfs_iops = {
	.lookup	=	tfs_iops_lookup,
};
static struct file_operations tfs_fops = {
	.owner	=	THIS_MODULE,
	.open	=	tfs_fops_open,
	.read	=	tfs_fops_read,
};

static struct file_operations tfs_dirops = {
	.owner	=	THIS_MODULE,
	.iterate	=	tfs_dirops_iterate,
};
#include "fstype_fun.h"
#include "fops.h"
#include "iops.h"
#include "sops.h"

static int tagfs_init(void){
	int ret;
	ret = register_filesystem(&tfs_type);
	if(unlikely(ret<0)){
		printk(KERN_INFO"Error registering the filesystem %s. Error- %d\n", tfs_type.name, ret);
	}else{	
		printk(KERN_INFO "filesystem %s registered successfully\n", tfs_type.name);
	}
	return 0;
}

static void tagfs_exit(void){
	int ret;
	ret = unregister_filesystem(&tfs_type);
	if(unlikely(ret<0)){
		printk(KERN_INFO"Error unregistering the filesystem %s. Error- %d\n", tfs_type.name, ret);
	}else{	
		printk(KERN_INFO "filesystem %s unregistered successfully\n", tfs_type.name);
	}
}
module_init(tagfs_init);
module_exit(tagfs_exit);
