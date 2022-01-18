NAME = bootloader
OBJS = $(NAME).o
LPI = libpi.small/libpi.a

ARM = arm-none-eabi
CC = $(ARM)-gcc
LD  = $(ARM)-ld
AS  = $(ARM)-as
OD  = $(ARM)-objdump
OCP = $(ARM)-objcopy

CFLAGS = -O -Wall -nostdlib -nostartfiles -ffreestanding -march=armv6 -std=gnu99  -Wno-unused-variable -Wno-unused-function -Ilibpi.small
ASFLAGS = --warn --fatal-warnings -mcpu=arm1176jzf-s -march=armv6zk

MEMMAP := ./libpi.small/memmap
START := ./staff-start.o

# overkill: add all the .h's as dependencies, memmap and the makefile.
DEPS := $(MEMMAP) $(wildcard ./*.h) ./Makefile 

all: lib $(START) $(OBJS) kernel.img

lib:
	@make -s -C libpi.small

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@
%.o: %.S $(DEPS)
	$(CC) -c $(CPP_ASFLAGS) $< -o $@

# rules to generate .elf->.list->.bin
%.elf: $(OBJS) $(MEMMAP) $(DEPS)  $(LPI)
	$(CC) $(CFLAGS) $(START) $(OBJS) -T $(MEMMAP) -o $@   $(LPI)

%.list: %.elf
	$(OD) -D  $< > $@

%.bin: %.list
	$(OCP) $*.elf -O binary $*.bin

# to reduce the chance of error.
kernel.img: $(NAME).bin
	mv $(NAME).bin kernel.img

clean:
	rm -rf *.o *.bin *~ tags *.bin *.elf *.list *.img Makefile.bak 
	@make -s -C ./libpi.small clean

.PHONY: all clean  lib

# stop make from deleting these suffix'd files.
.PRECIOUS: %.list %.elf
