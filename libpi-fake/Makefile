TARGET = libpi-fake.a

LPP = $(CS140E_2022_PATH)

# define where gpio.c is, and the programs you want to run.
GPIO := $(LPP)/libpi/src/gpio.c $(LPP)/libpi/src/gpio-int.c

# GPIO := ../libpi/staff-private/gpio.c ../libpi/staff-private/gpio-int.c 

# if you want to compile everything in a directory, do something like this.
# SOURCES := $(wildcard ./*.c)

# include uart, include gpio.

SRC :=  fake-put-get.c fake-random.c pi-random.c  fake-driver.c fake-delay.c fake-dev-barrier.c $(GPIO) fake-printk.c fake-reboot.c fake-usec-delay.c
OBJS := $(patsubst %.c, %.o, $(SRC))
OBJS := $(foreach obj, $(OBJS), ./$(notdir $(obj)))

dep := $(OBJS:.o=.d)  # one dependency file for each source

VPATH := $(sort $(dir $(SRC)))


ifndef CS140E_2022_PATH
$(error CS140E_2022_PATH is not set: this should contain the absolute path to where this directory is.  Define it in your shell's initialiation.  For example, .tcshrc for tcsh or .bashrc for bash)
endif


# where the libpi includes are
LPI = $(LPP)/libpi/include
# where libunix is (currently)
LUNIX = $(LPP)/libunix

CC=gcc
CFLAGS = -Wall -g -Og -std=gnu99  -Werror -Wno-unused-function 
# added the include path for libpi-fake
CFLAGS += -I$(LPI)  -I$(LUNIX) -DRPI_UNIX -I$(LPP)/libpi-fake

# overdepend: all .o's depend on all .h's  --- make deps as simple as possible.
DEPS := $(wildcard ./*.h) $(LPI)/rpi.h Makefile

all: $(TARGET)

lib:
	echo "Deps=$(DEPS)"

$(TARGET): $(OBJS) $(DEPS)
	ar cr $(TARGET) $(OBJS) 

-include $(dep)  # include dep files

#./%.o: %.c ./%.d
#	$(CC) $(CFLAGS) -c $< -o $@

./%.d: %.c ./Makefile
	$(CC) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

# $(OBJS): $(DEPS) 

#%.o: %.c $(DEPS)
#	$(CC) $(CFLAGS) -c $< -o $(notdir $@)


check: $(TARGET)
	make -C tests-gpio check
	make -C tests-uart check

clean:
	rm -f *.o *.d $(TARGET) *~  *.bak  *.out $(OBJS)
	make -C tests-gpio clean
	make -C tests-uart clean

.PHONY: all clean
