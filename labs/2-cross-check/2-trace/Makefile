# need to fix this makefile

# for rewriting we just do a few programs.  all in this directory.  we use
# the objects in staff-objs.  compile a program both with and without 
# tracing.

# "make run": runs the programs listed in tests/Makefile
# "make check": checks the programs listed in tests/Makefile
# "make emit": emits new output files for the programs listed in tests/Makefile

# if you want to trace main as well, add it as a file.
OBJS    := trace-simple.o trace-notmain.o
# this is passed to ld
WRAP = --wrap=PUT32 --wrap=GET32

# check that your path is defined.
ifndef CS140E_2022_PATH
$(error CS140E_2022_PATH is not set: this should contain the absolute path to where this directory is.  Define it in your shell's initialiation.  For example, .tcshrc for tcsh or .bashrc for bash)
endif

include $(CS140E_2022_PATH)/libpi/defs.mk
CFLAGS += -Wno-unused-function -Wno-unused-variable

# add all the .h's as dependencies, libpi and the makefile.
DEPS := $(MEMMAP) $(LPI) $(wildcard ./*.h) ./Makefile 

LIBS = $(LPI)  

all: $(OBJS) 

run:
	make -C tests  runall
check:
	make -C tests  checkall
emit:
	make -C tests  emit

$(LPI):
	make -C $(LPP)

$(OBJS): $(DEPS)
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@
%.o: %.S
	$(CC) -c $(CPP_ASFLAGS) $< -o $@

$(NAME).elf: $(NAME).o $(OBJS) $(MEMMAP) $(DEPS) 
	$(LD) $(WRAP) $(START) $(OBJS) -T $(LPP)/memmap -o $@  $(LIBS)

%.list: %.elf
	$(OD) -D   $*.elf > $*.list
%.bin: %.list
	$(OCP) $*.elf -O binary $*.bin

tags: 
	ctags *.[chSs] $(LPP)/*.[ch] $(LPP)/*.[chSs]

clean:
	rm -rf $(NAME) *~ tags *.o *.bin *.elf *.list *.img Makefile.bak  *.d

.PHONY: libs all tags clean run check emit

# stop make from deleting these suffix'd files.
.PRECIOUS: %.list %.elf
