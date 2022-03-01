TEST_SRC := hello-f.c 

# use our own custom cstart
SUPPORT_OBJS := loader-cstart.o
# override memmap
MEMMAP = ./memmap.fixed
# override start 
START = loader-start.o

# define this if you need to give the device for your pi
TTYUSB = 
BOOTLOADER = my-install

# uncomment if you want it to automatically run.
RUN=1

include $(CS140E_2022_PATH)/libpi/mk/Makefile.template

# add more actions to clean
clean::
	rm -f *.o
