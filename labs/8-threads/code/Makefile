# define this if you need to give the device for your pi
TTYUSB = 
BOOTLOADER = my-install
GREP_STR := 'TRACE:\|THREAD:\|ERROR:\|PANIC:' 

# uncomment if you want it to automatically run.
RUN=1

TEST_SRC := 0-test-push.c # 4-test-exit.c # 1-test-thread.c 3-test-cswitch.c
TEST_SRC := $(wildcard ./[0]-test*.c)
TEST_SRC := $(wildcard ./[0]*.c)
TEST_SRC := 0-where-push.c 0-write-regs.c
TEST_SRC := $(wildcard ./[1]-test*.c)
TEST_SRC := $(wildcard ./[4]-test*.c)
TEST_SRC := $(wildcard ./[5]-test*.c)
TEST_SRC := $(wildcard ./[2]-*.c)
TEST_SRC := $(wildcard ./*-test-*.c)
TEST_SRC := $(wildcard ./[1]-*.c)

# put all your miscellaneous assembly in this file. 
ifdef CS140E_STAFF
SUPPORT_OBJS := lab-c-helpers.o 
else
SUPPORT_OBJS := lab-c-helpers.o rpi-thread.o  thread-asm.o
endif

include $(CS140E_2022_PATH)/libpi/mk/Makefile.template

# include Makefile.template
