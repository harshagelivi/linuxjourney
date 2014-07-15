#include<stdio.h>
#include<stdint.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>

int main(int argc, char *argv[]){
	int fd, numbytes, offset, zfd;
	if( (fd=open(argv[1], O_RDWR)) == -1){
		printf("Cant open the file %s\n", argv[1]);
		return -1;
	};
	char buff[60];
	numbytes = read(fd, buff, 60);
	printf("%s\n", buff);
	close(fd);	
	return 0;
}
