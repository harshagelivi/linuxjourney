#include<linux/module.h>
#include<linux/string.h>
#include<linux/fs.h>
#include<asm/uaccess.h>
#include<asm/errno.h>
#include<linux/kernel.h>
#include<linux/cdev.h>


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Char Device");
MODULE_AUTHOR("Gelivi");

#define DEVICE_NAME "chardev"
#define BUFF_LEN 80

struct cdev * my_cdev;
static int Major;
static int Device_Open=0;
static char msg[BUFF_LEN];
static char * msg_ptr;

static int chardev_open(struct inode *, struct file *);
static int chardev_release(struct inode *, struct file *);
static ssize_t chardev_read(struct file *, char *, size_t, loff_t *);
static ssize_t chardev_write(struct file *,const char *, size_t, loff_t *);

static struct file_operations fops=
{
	.owner = THIS_MODULE,
	.read = chardev_read,
	.open = chardev_open,
	.write = chardev_write,
	.release = chardev_release
};

static int __init chardev_init(void){
	dev_t dev = MKDEV(0, 0);
	int result = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
	Major = MAJOR(dev);
	if (result < 0){
		printk(KERN_INFO "device registration failed\n");
		return result;
	}	

	printk(KERN_INFO "iam created with major %d.\n", Major);
	printk(KERN_INFO "'mknod /dev/chardev c %d 0'.\n", Major);
	
	my_cdev = cdev_alloc();
   	my_cdev->owner = THIS_MODULE;
   	my_cdev->ops = &fops;
	result = cdev_add (my_cdev, dev, 1);
	if (result)
		printk(KERN_INFO "Error %d adding device", result);


    return 0;
}

static void __exit chardev_cleanup(void){
	cdev_del(my_cdev);
	unregister_chrdev_region(MKDEV(Major, 0), 1);
	printk(KERN_INFO "Successfully removed the module\n");
}
static int chardev_open(struct inode * inode, struct file * filp){
	static int counter =0;
	if(Device_Open) return -EBUSY;
	Device_Open++;
	sprintf(msg, "iam opened %d times.\n", counter++);
	msg_ptr=msg;
	return 0;

}
static int chardev_release(struct inode * inode, struct file *  filp){
	Device_Open--;
	return 0;

}
static ssize_t chardev_read(struct file * filp, char * buffer, size_t length, loff_t * offset){
	int bytes_read=0;
	if(*msg_ptr==0) return 0;
	while(length && *msg_ptr){
		put_user(*(msg_ptr++), buffer++);
		length--;
		bytes_read++;
	}
	return bytes_read;

}
static ssize_t chardev_write(struct file * filp, const char * buff, size_t len, loff_t * off){
	printk(KERN_INFO "write isnt suppported yet\n");
	return -EINVAL;
}

module_init(chardev_init);
module_exit(chardev_cleanup);
