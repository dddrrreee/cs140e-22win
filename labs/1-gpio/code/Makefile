PROGS = 1-blink.bin 2-blink.bin 3-input.bin
OBJS = start.o 1-blink.o gpio.o

ARM = arm-none-eabi
CC = $(ARM)-gcc
LD  = $(ARM)-ld
AS  = $(ARM)-as
OD  = $(ARM)-objdump
OCP = $(ARM)-objcopy

CFLAGS = -O -Wall -nostdlib -nostartfiles -ffreestanding  -march=armv6 -std=gnu99  -Wno-unused-variable
ASFLAGS = --warn --fatal-warnings -mcpu=arm1176jzf-s -march=armv6zk

MEMMAP := ./memmap
START := ./start.o

# overkill: add all the .h's as dependencies, memmap and the makefile.
DEPS := $(MEMMAP) $(wildcard ./*.h) ./Makefile 

all:  $(START) $(OBJS) $(PROGS)

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@
%.o: %.S
	$(CC) -c $(CPP_ASFLAGS) $< -o $@

# rules to generate .elf->.list->.bin
%.elf: %.o gpio.o $(MEMMAP) $(DEPS) 
	$(CC) $(CFLAGS) $(START) $< gpio.o -T $(MEMMAP) -o $@ 

%.list: %.elf
	$(OD) -D  $< > $@

%.bin: %.list
	$(OCP) $*.elf -O binary $*.bin

clean:
	rm -rf *.o *.bin *~ tags *.bin *.elf *.list *.img Makefile.bak 

.PHONY: all clean 

# stop make from deleting these suffix'd files.
.PRECIOUS: %.list %.elf
