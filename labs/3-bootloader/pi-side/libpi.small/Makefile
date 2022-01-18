LIB_OBJS = reboot.o timer.o helpers.o mem-barrier.o
EXTRA= gpio.o gpioextra.o my-uart.o
SRC = $(LIB_OBJS:.o=.c)

ARM = arm-none-eabi
CC = $(ARM)-gcc
LD  = $(ARM)-ld
AS  = $(ARM)-as
OD  = $(ARM)-objdump
OCP = $(ARM)-objcopy
CFLAGS = -Wall -nostdlib -nostartfiles -ffreestanding  -march=armv6 -Og -std=gnu99

TARGET = libpi.a

all : $(TARGET) 

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@ 

%.o: %.c
	$(CC) $(CFLAGS) -c $< 

$(TARGET): $(LIB_OBJS)  $(EXTRA)
	arm-none-eabi-ar crf libpi.a $(LIB_OBJS) $(EXTRA)
	$(OD) -D $@ > $@.list

clean :
	rm -f $(LIB_OBJS) start.o *.bin *.hex *.elf *.list *.img *.bc *.clang.s *~ $(TARGET)  Makefile.bak

depend: 
	makedepend $(SRC)
# DO NOT DELETE

reboot.o: rpi.h
timer.o: rpi.h
uart.o: gpio.h mem-barrier.h
