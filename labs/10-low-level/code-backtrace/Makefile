TEST_SRC := test-backtrace.c
SUPPORT_OBJS := 

# define this if you need to give the device for your pi
TTYUSB = 
BOOTLOADER = my-install

# uncomment if you want it to automatically run.
RUN=1


GREP_STR := 'TRACE:\|THREAD:\|ERROR:\|PANIC:' 

# CFLAGS_EXTRA =  -fno-omit-frame-pointer -g -funwind-tables -fno-exceptions
CFLAGS_EXTRA =  -fno-omit-frame-pointer -mpoke-function-name

include $(CS140E_2022_PATH)/libpi/mk/Makefile.template
