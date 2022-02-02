SUPPORT_OBJS := lab-c-helpers.o 

# comment this out to use your code
USE_STAFF=1

ifeq ($(USE_STAFF),1)
STAFF_OBJS := staff-rpi-thread.o  staff-thread-asm.o
else
SUPPORT_OBJS += rpi-thread.o  thread-asm.o
endif

# define this if you need to give the device for your pi
TTYUSB = 
BOOTLOADER = my-install

# uncomment if you want it to automatically run.
# RUN=1

# part 1
TEST_SRC := $(wildcard ./1*.c)
# part 2
TEST_SRC := $(wildcard ./2*.c)
# ...
TEST_SRC := $(wildcard ./4*.c)
TEST_SRC := $(wildcard ./5*.c)
TEST_SRC := $(wildcard ./6*.c)
TEST_SRC := $(wildcard ./7*.c)

# to run all the tests.
TEST_SRC := $(wildcard ./[0-9]*.c)

# should not have to change below



GREP_STR := 'TRACE:\|THREAD:\|ERROR:\|PANIC:' 

# CAN_EMIT = 1

include $(CS140E_2022_PATH)/libpi/mk/Makefile.template

# override the default rules.  should have a cleaner solution --- we want to add the 
# push-asm and write-regs asm to the link line.
2-where-push.elf: 2-where-push.o 2-where-push-asm.o $(ALL_OBJS) $(DEPS) $(START) $(LIBS)
	$(LD) $(START)  2-where-push.o 2-where-push-asm.o $(ALL_OBJS) -T $(MEMMAP) -o $@  $(LIBS)

2-write-regs.elf: 2-write-regs.o 2-write-regs-asm.o $(ALL_OBJS) $(DEPS) $(START) $(LIBS)
	$(LD) $(START)  2-write-regs.o 2-write-regs-asm.o $(ALL_OBJS) -T $(MEMMAP) -o $@  $(LIBS)

clean::
	rm -f rpi-thread.o thread-asm.o
	rm -f 2-write-regs-asm.o 2-where-push-asm.o 
