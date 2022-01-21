TEST_SRC = 0char-inf.c  1hello-inf.c  2hello-once.c
TEST_OBJS := $(patsubst %.c, %.o, $(TEST_SRC))
TEST_PROGS := $(patsubst %.c, %.bin, $(TEST_SRC))

# check that your path is defined.
ifndef CS140E_2022_PATH
$(error CS140E_2022_PATH is not set: this should contain the absolute path to where this directory is.  Define it in your shell's initialiation.  For example, .tcshrc for tcsh or .bashrc for bash)
endif

include $(CS140E_2022_PATH)/libpi/defs.mk
CFLAGS += -Wno-unused-function -Wno-unused-variable -falign-functions=4 

# add all the .h's as dependencies, libpi and the makefile.
DEPS := $(MEMMAP) $(LPI) $(wildcard ./*.h) ./Makefile 

LIBS = $(LPI)  

all: libs $(TEST_PROGS)

libs: 
	make -C $(LPP)

$(OBJS): $(DEPS)
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@
%.o: %.S
	$(CC) -c $(CPP_ASFLAGS) $< -o $@

%.elf: %.o $(MEMMAP) $(DEPS) 
	$(LD) $(START) $< -T $(LPP)/memmap -o $@  $(LIBS)

%.list: %.elf
	$(OD) -D   $*.elf > $*.list
%.bin: %.list
	$(OCP) $*.elf -O binary $*.bin

tags: 
	ctags *.[chSs] $(LPP)/*.[ch] $(LPP)/*.[chSs]

clean:
	rm -rf $(NAME) *~ tags *.o *.bin *.elf *.list *.img Makefile.bak  *.d

.PHONY: libs all tags clean run

# stop make from deleting these suffix'd files.
.PRECIOUS: %.list %.elf
