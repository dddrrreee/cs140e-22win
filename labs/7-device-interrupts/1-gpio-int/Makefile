TEST_SRC = 1-gpio-int-ex.c 2-logic-analyzer.c

# comment out interrupts-asm when you put in your own.
STAFF_OBJS += interrupts-asm.o 

STAFF_OBJS += sw-uart.o

# if you need to override the install can change this (include the
# device name if you need it)
BOOTLOADER = my-install

# automatically run the code
RUN = 1

include $(CS140E_2022_PATH)/libpi/mk/Makefile.template
