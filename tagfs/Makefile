obj-m := tagfs.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:ko 

ko: tagfs.c decls.h prototypes.h fstype_fun.h
	$(MAKE) -C $(KDIR) M=$(PWD) modules
 
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
