###To check tagfs:<br/>
Create a file to mount:<br/>
**`dd bs=1M count=256 if=/dev/zero of=image`**<br/>
Format the file:<br/>
**`gcc mkfs-tfs -o mkfs-tfs`**
**`./mkfs-tfs image`**
Creating a directory to mount the above file:<br/>
**`mkdir mtpt`**<br/>


To compile:<br/>
**`make`**<br/>
To insert this module: <br/>
**`sudo insmod tagfs.ko `**<br/>

To mount that file on the mount directory:<br/>
**`sudo mount -o loop -t tagfs image /path/to/that/mount/dir `**<br/>
To access mount point:<br/>
**`sudo chmod 777 mount `**<br/>
To unmount: <br/>
**`sudo unmount  /path/to/that/mount/dir `**<br/>

To remove this module: <br/>
**`sudo rmmod tagfs `**<br/>

To see output of printk(kernel ring buffer):
**`dmesg `**<br/>


