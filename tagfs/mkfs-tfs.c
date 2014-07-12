#include<stdio.h>
#include<stdint.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include "decls.h"

char pad[TFS_BLOCK_SIZE];

int main(int argc, char *argv[]){
	struct tfs_super_block sb;
	struct tfs_inode inode;
	int fd, numbytes, offset, zfd;
	if(argc!=2){
		printf("Usage: ./mkfs-tfs <device>\n");
		return 1;
	}
	if( (fd=open(argv[1], O_WRONLY)) == -1){
		printf("Cant open the device\n");
		return -1;
	};
	
	sb.s_magic = TFS_MAGIC;
	sb.s_block_size = TFS_BLOCK_SIZE;
	
	numbytes = write(fd, (void *)&pad, sizeof(pad));	
	printf("Boot block written - %d\n", numbytes);	

	numbytes = write(fd, (void *)&sb, TFS_BLOCK_SIZE);	
	printf("Super block bytes written - %d\n", numbytes);	

	pad[2]=1;
	numbytes = write(fd, (void *)&pad, sizeof(pad));	
	printf("Inode map written - %d\n", numbytes);	
	pad[2]=0;

	inode.i_mode = S_IFDIR;
	inode.i_ino = TFS_ROOT_INO;
	inode.i_child_count = 1;
	inode.i_blocks = 1;
	inode.i_block[0] = TFS_ROOT_BLOCK;
	offset = lseek(fd, TFS_ROOT_INO*sizeof(struct tfs_inode), SEEK_CUR);
	numbytes = write(fd, (void *)&inode, sizeof(inode));	
	printf("Root inode written - %d\n", numbytes);
	
	inode.i_mode = S_IFREG;
	inode.i_ino = 3;
	inode.i_blocks = 1;
	inode.i_block[0] = 9;
	numbytes = write(fd, (void *)&inode, sizeof(inode));	
	printf("file1 inode written - %d\n", numbytes);

	offset = lseek(fd, 5*TFS_BLOCK_SIZE, SEEK_SET);
	pad[0]=pad[1]=pad[2]=pad[3]=pad[4]=pad[5]=pad[8]=1;
	numbytes = write(fd, (void *)&pad, sizeof(pad));	
	printf("Block map written - %d\n", numbytes);
	pad[0]=pad[1]=pad[2]=pad[3]=pad[4]=pad[5]=pad[8]=0;

	offset = lseek(fd, TFS_ROOT_BLOCK * TFS_BLOCK_SIZE, SEEK_SET);
	struct tfs_dir_entry entry;
	entry.ino = 3;
	entry.name_len = 6;
	strcpy(entry.name, "file1");
	numbytes = write(fd, (void *)&entry, sizeof(entry));
	printf("Dir entry for file1 is written - %d\n", numbytes);

	close(fd);	
	return 0;
}
