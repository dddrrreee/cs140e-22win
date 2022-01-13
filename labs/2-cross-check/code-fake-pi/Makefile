# where our GPIO code is
DIR = ../../1-gpio/code

# the raw, real source code that we include.
REAL_SRC := $(DIR)/gpio.c

# our name for the library
TARGET = libpi-fake.a

# the different code we want to test.
PROG_SRC = $(DIR)/1-blink.c $(DIR)/2-blink.c $(DIR)/3-input.c

PROG_OBJS := $(PROG_SRC:.c=.o)
PROG_OBJS := $(foreach obj, $(PROG_OBJS), ./$(notdir $(obj)))
PROGS := $(patsubst %.o, ./%, $(PROG_OBJS))

# grab all .c files in the current directory
SRC :=  $(REAL_SRC) $(wildcard ./*.c)
OBJS := $(patsubst %.c, %.o, $(SRC))
OBJS := $(foreach obj, $(OBJS), ./$(notdir $(obj)))

dep := $(OBJS:.o=.d)  # one dependency file for each source

# automatically give make the directories to look in.
VPATH := $(sort $(dir $(SRC)))

CC=gcc
CFLAGS = -Wall -g -std=gnu99  -Werror -Wno-unused-function -Wno-unused-variable
# added the include path for libpi-fake
CFLAGS += -I$(DIR)

DEPS := ./Makefile

all : $(OBJS) $(PROG_OBJS) $(PROGS) $(TARGET) # $(OBJS) 

# fix this rule: can't delete the targets in <all>
./%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $(notdir $@)

$(TARGET): $(OBJS) $(DEPS)
	ar cr $(TARGET) $(OBJS) 

./%: ./%.o $(OBJS) $(DEPS) 
	$(CC) $(CFLAGS) $(OBJS) $< -o $@ 

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),cleanall)
-include $(dep)  # include dep files
endif
endif

./%.d: %.c
	$(CC) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

clean :
	rm -f *.o *.d *~ Makefile.bak $(NAME) *.pitag  $(PROGS) $(TARGET)
	make -C tests clean

.PHONY: clean tags libs run check 
