# if you do:
#   - "make checkall" it will check all programs in TESTS against their .out
#   - "make emitall" it emit .outs for all programs in TESTS
#   - "make runall" it will run all programs in TESTS
# if you want to just grab all tests do:
TESTS = test2-swi
TESTS = 0-test-nop 1-test-hello
TESTS = 0-test-nop.c
TESTS := $(wildcard ./[0-3]*-test-*.c)

# if you want to run with register checking.
TYPE= reg-equiv.

USE_STAFF=1
ifeq ($(USE_STAFF),1)
OS_PATH = ../trivial-os.staff
else
OS_PATH = ../trivial-os
endif
OS_BIN = $(OS_PATH)/trivial-os.bin

# if you want to compile everything in a directory, do something like this.
# SOURCES := $(wildcard ./*.c)

include $(CS140E_2022_PATH)/libpi/defs.mk

MEMMAP := ./memmap
START := ./start.o
SYS_OBJS = ./trivial-user-level.o  ./trivial-user-level-asm.o

GREP_STR="TRACE:EQUIV:"

OBJS := $(patsubst %.c, %.o, $(ALL_SOURCES))  $(SYS_OBJS) $(START)

TESTS := $(patsubst %.c, %, $(TESTS)) 

TEST_BINARIES := $(patsubst %, %.bin, $(TESTS))  
TEST_RUN := $(patsubst %, %.run, $(TESTS))  
TEST_EMIT := $(patsubst %, %.emit, $(TESTS))  
TEST_CHECK := $(patsubst %, %.check, $(TESTS))  
TEST_PITAG := $(patsubst %, %.ptag, $(TESTS))  


CFLAGS += -Wno-unused-function -Wno-unused-variable -fno-toplevel-reorder -Werror  -I$(OS_PATH) -I.

# -fno-omit-frame-pointer -mpoke

# -fno-reorder-functions

# add all the .h's as dependencies, libpi and the makefile.
DEPS := $(MEMMAP) $(LPI) $(LIBM) $(wildcard ./*.h) ./Makefile 

LIBS =

all: libs  $(OBJS) $(TEST_PITAG)  $(TEST_RUN) # $(TEST_CHECK)

libs: 
	@make -C $(OS_PATH)
	@make -C pitag-linker

$(OBJS): $(DEPS)
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@
%.o: %.S
	$(CC) -c $(CPP_ASFLAGS) $< -o $@

# rules to generate .elf->.list->.bin
%.elf: %.o $(SYS_OBJS) $(MEMMAP) $(DEPS) 
	$(LD) $(START) $*.o $(SYS_OBJS) -T $(MEMMAP) -o $@  $(LIBS)

%.list: %.elf
	@$(OD) -D   $*.elf > $*.list

%.bin: %.list
	@$(OCP) $*.elf -O binary $*.bin

# need to fix the dependency --- will remake the damn thing.
%.ptag: %.bin 
	@./pitag-linker/pitag-linker $(OS_BIN) $*.bin $*.ptag

%.run: %.ptag
	@./pitag-linker/pitag-linker $(OS_BIN) $*.bin $*.ptag
	my-install ./$*.ptag

# note: this will fail if output does not contain a trace.
# we pull everything into .out so that it's easier to debug.
%.emit: %.ptag
	@./pitag-linker/pitag-linker $(OS_BIN) $*.bin $*.ptag
	my-install ./$*.ptag 2>&1 > /dev/null | cat > ./$*.$(TYPE)out

%.check: %.ptag
	@./pitag-linker/pitag-linker $(OS_BIN) $*.bin $*.ptag
	my-install ./$*.ptag 2>&1 > /dev/null | grep $(GREP_STR)  > ./$*.test
	grep $(GREP_STR) ./$*.$(TYPE)out | diff ./$*.test -
	rm ./$*.test

# not sure if there is a better way: use these fake target to run our
# checks for everything.
emitall: $(TEST_EMIT) $(TEST_BINARIES)
checkall: $(TEST_CHECK) $(TEST_BINARIES)
runall: $(TEST_RUN) 

tags: 
	ctags *.[chSs] $(LPP)/*.[ch] $(LPP)/*.[chSs] 

clean:
	rm -rf $(BUILD_DIR) $(NAME) *~ tags $(OBJS) *.elf  *.img Makefile.bak  *.d *.test *.o *.ptag
	make -C pitag-linker clean

cleanall:
	make -C ./ clean
	rm -f *.out *.o *.bin *.list

.PHONY: libs all tags clean run trivial-libpi  runall emitall checkall update

# stop make from deleting these suffix'd files.
.PRECIOUS: %.list %.elf %.ptag
