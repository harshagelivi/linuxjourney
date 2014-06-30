#Writing a Hello World! module in linux kernel
====================================

##Tried and tested in Ubuntu 12.04 and 14.04:

1.Download this zip file and extract.

2.Open terminal and cd into that folder.

The commands to be run:

```sudo make```

```sudo insmod hello.ko      #to insert the module```

```tail /var/log/syslog       #you can see "hello world!" printed```

```sudo rmmod hello          #to remove module```

```tail /var/log/syslog      #you can see gudbye message printed ```

```lsmod                    # to list the modules```

```modinfo hello.ko         #to get info abt that module ```


