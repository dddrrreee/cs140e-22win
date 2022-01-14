PROGS = hello.bin
# use
GPIO = gpio.o
START := ./start.o
ALL_OBJS := $(OBJS) $(patsubst %.bin, %.o, $(PROGS))

ARM = arm-none-eabi
CC = $(ARM)-gcc
LD  = $(ARM)-ld
AS  = $(ARM)-as
OD  = $(ARM)-objdump
OCP = $(ARM)-objcopy

LIBS = libpi.a
CFLAGS = -O -Wall -nostdlib -nostartfiles -ffreestanding  -march=armv6 -std=gnu99  -Wno-unused-variable
ASFLAGS = --warn --fatal-warnings -mcpu=arm1176jzf-s -march=armv6zk

MEMMAP := ./memmap

# overkill: add all the .h's as dependencies, memmap and the makefile.
DEPS := $(MEMMAP) $(wildcard ./*.h) ./Makefile 

all:  $(START) $(GPIO) $(ALL_OBJS) $(PROGS)

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@
%.o: %.S $(DEPS)
	$(CC) -c $(CPP_ASFLAGS) $< -o $@

%.elf: %.o $(GPIO) $(DEPS) $(START) $(LIBS) 
	$(LD) $(START)  $< $(GPIO) -T $(MEMMAP) -o $@ $(LIBS)

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),cleanall)
-include $(dep)  # include dep files
endif
endif

./%.d: %.c $(DEPS)
	$(CC) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@
%.list: %.elf
	$(OD) -D   $*.elf > $*.list
%.bin: %.list
	$(OCP) $*.elf -O binary $*.bin

clean:
	rm -rf *.bin *~ tags *.bin *.elf *.list *.img Makefile.bak hello.o gpio.o  start.o

.PHONY: all clean 

# stop make from deleting these suffix'd files.
.PRECIOUS: %.list %.elf
