obj-m := demo.o
LINUX :=/lib/modules/$(shell uname -r)/build

all:
	make -C $(LINUX) M=$(PWD)/module modules

clean:
	make -C $(LINUX) M=$(PWD)/module clean
