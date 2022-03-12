CONFIG_MODULE_SIG=n
obj-m = kmod.o
KERNEL = $(shell uname -r)

all:
	make -C /lib/modules/$(KERNEL)/build M=$(shell pwd) modules
	gcc cmod.c -o cmod

clean:
	make -C /lib/modules/$(KERNEL)/build M=$(shell pwd) clean
	rm cmod
