KDIR = /lib/modules/`uname -r`/build

pceset: pceset.c
	make -C $(KDIR) M=`pwd`

clean:
	make -C $(KDIR) M=`pwd` clean
