obj-m = sample.o
all:
	make -C /lib/modules/5.15.0-105-generic/build/ M=/home/ramdisk/git/bhargavi modules
clean:	
	make -C /lib/modules/5.15.0-105-generic/build M=/home/ramdisk/git/bhargavi clean
	

