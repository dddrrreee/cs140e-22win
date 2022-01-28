# trivial makefile: can define the SRC here or pass it in.
# note: this is useful or dangerous: we strip out all the 
# directory stuff so we can put the executable here.

MAKEFLAGS += --no-print-directory

TEST_SRC := $(wildcard ./[0125]-*.c)                      \
            $(wildcard ./[34]-unix-*.c)   

TEST_SRC := $(wildcard ./[67]-*.c) 

# make them local to this directory
OBJS := $(patsubst %.c, %.o, $(TEST_SRC))
OBJS := $(foreach obj, $(OBJS), ./$(notdir $(obj)))

TEST_PROGS := $(patsubst %.o, %, $(OBJS))

dep := $(OBJS:.o=.d)  # one dependency file for each source


# pull out all the directories in the test files so make knows 
# where to look for everything.
# note: if tests have the same name, you'll get a collision.
VPATH := $(sort $(dir $(TESTS)))

LPP = $(CS140E_2022_PATH)/libpi
LIBPI_FAKE = ../libpi-fake.a
CC = gcc

# where libunix is (currently)
LUNIX = $(LPP)/libunix

# where the libpi includes are
CFLAGS =  -Wall -Wno-unused-function -Wno-unused-variable 
CFLAGS += -I$(LPP)/ -I$(LPP)/include -I$(LUNIX) 
# -DRPI_UNIX

DEPS = ./Makefile $(LIBPI_FAKE) 

all: libs $(TEST_PROGS) 

libs:
	make -s -C ../  

## we put all the executables here: how to remove?
./%: %.o  $(DEPS) 
	$(CC) $(CFLAGS) $< -o $@  $(LIBPI_FAKE)

-include $(dep)  # include dep files

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

.PHONY: clean

clean:
	rm -rf $(TEST_PROGS) *~ tags *.o Makefile.bak *.d

cleanall:
	@make -s -f ./Makefile clean
	@make -s -f ./Makefile.test cleanall TESTS=x

.PHONY: all clean libs run check emit
