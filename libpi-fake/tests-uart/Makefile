# trivial makefile: can define the SRC here or pass it in.
# note: this is useful or dangerous: we strip out all the 
# directory stuff so we can put the executable here.

# the path to your uart.c: should be in libpi/src
PI_SRC = $(CS140E_2022_PATH)/libpi/src/uart.c

# the tests to run: we'll add some.
TEST_SRC := $(wildcard ./[01]-*.c) 

MAKEFLAGS += --no-print-directory

# make them local to this directory
OBJS := $(patsubst %.c, %.o, $(TEST_SRC)) 
OBJS := $(foreach obj, $(OBJS), ./$(notdir $(obj)))

PI_OBJS := $(patsubst %.c, %.o, $(PI_SRC))
PI_OBJS := $(foreach obj, $(PI_OBJS), ./$(notdir $(obj)))

TEST_PROGS := $(patsubst %.o, %, $(OBJS))

# one dependency file for each source
dep := $(OBJS:.o=.d)   $(PI_OBJS:.o=.d)

# pull out all the directories in the test files so make knows 
# where to look for everything.
# note: if tests have the same name, you'll get a collision.
VPATH := $(sort $(dir $(TEST_SRC))) $(sort $(dir $(PI_SRC)))

LPP = $(CS140E_2021_PATH)/libpi
LIBPI_FAKE = ../libpi-fake.a
LPI = -I$(LPP)/include

CC = gcc
CFLAGS = -DRPI_UNIX $(LPI) -I$(LPP) -I../ -Wall -Wno-unused-function -Wno-unused-variable 

DEPS = ./Makefile $(LIBPI_FAKE) 

all: libs $(TEST_PROGS) 

libs:
	@make -s -C ../  

## we put all the executables here: how to remove?
./%: %.o $(PI_OBJS) $(DEPS) 
	$(CC) $(CFLAGS) $< -o $@  $(PI_OBJS) $(LIBPI_FAKE)

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),cleanall)
-include $(dep)  # include dep files
endif
endif

%.d: %.c
	@$(CC) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

# i'm not sure this actually makes things better.
emit: $(DEPS) $(TEST_PROGS)
	@make -f ./Makefile.test emitall "TESTS=$(TEST_PROGS)"
run: $(DEPS) $(TEST_PROGS)
	@make -f ./Makefile.test runall "TESTS=$(TEST_PROGS)"
check: $(DEPS) $(TEST_PROGS)
	@make -f ./Makefile.test checkall "TESTS=$(TEST_PROGS)"
cksum: $(DEPS) $(TEST_PROGS)
	@make -f ./Makefile.test cksumall "TESTS=$(TEST_PROGS)"

clean:
	rm -rf $(TEST_PROGS) *~ tags *.o Makefile.bak *.d

cleanall:
	@make -s -f ./Makefile clean
	@make -s -f ./Makefile.test cleanall TESTS=x

.PHONY: all clean libs run check emit cleanall
