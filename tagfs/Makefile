obj-m := tagfs.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:ko tagfs_mkfs

ko:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	
tagfs_mkfs_SOURCES:
		tagfs_mkfs.c tagfs.h
 
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
