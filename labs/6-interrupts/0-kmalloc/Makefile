USE_LOCAL_KMALLOC = ./kmalloc.c
OPT_LEVEL = -O2

# the tests you want to run: can list them out.
TEST_SRC = $(sort $(wildcard ./[0-9]*-test*.c))

MAKEFLAGS += --no-print-directory

TEST_OBJS := $(patsubst %.c, %.o, $(TEST_SRC))
TEST_PROGS := $(patsubst %.c, %.bin, $(TEST_SRC))

OBJS := $(USE_LOCAL_KMALLOC:.c=.o)

dep := $(TEST_OBJS:.o=.d) $(OBJS:.o=.d) 

# VPATH := $(sort $(dir $(SRC)))

include $(CS140E_2022_PATH)/libpi/defs.mk
CFLAGS += -Wno-unused-function -Wno-unused-variable 

# add the memmap file, libpi and the makefile as dependencies
DEPS := $(MEMMAP) $(LPI) ./Makefile 

LIBS = $(LPI)  

all: libs $(TEST_PROGS) 

libs: 
	make -C $(LPP)

$(OBJS): $(DEPS)
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@
%.o: %.S
	$(CC) -c $(CPP_ASFLAGS) $< -o $@

%.elf: %.o $(OBJS) $(DEPS) 
	$(LD) $(START) $< $(OBJS) -T $(MEMMAP) -o $@  $(LIBS)

# *unbelievable*: make clean doesn't skip this include even though the 
# target is after.
#   https://www.gnu.org/software/make/manual/html_node/Goals.html
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

tags: 
	ctags *.[chSs] $(LPP)/*.[ch] $(LPP)/*.[chSs]


# should not re-emit.
#emit: $(DEPS) $(TEST_PROGS)
#	@make -f ./Makefile.check emit "TESTS=$(TEST_PROGS)"
run: $(DEPS) $(TEST_PROGS)
	@make -f ./Makefile.check run "TESTS=$(TEST_PROGS)"
check: $(DEPS) $(TEST_PROGS)
	@make -f ./Makefile.check check "TESTS=$(TEST_PROGS)"

clean:
	rm -rf $(NAME) *~ tags *.o *.bin *.elf *.list *.img Makefile.bak  *.d 
cleanall:
	@make -C . clean
	rm -f *.out

.PHONY: libs all tags clean run

# stop make from deleting these suffix'd files.
.PRECIOUS: %.list %.elf %.o
