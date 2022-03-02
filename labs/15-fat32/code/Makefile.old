# the tests you want to run when you call `make test`
TEST_SRC = $(wildcard tests/[0]-*.c)
# TEST_SRC = $(wildcard tests/[01]-*.c)
# TEST_SRC = $(wildcard tests/[012]-*.c)
# TEST_SRC = $(wildcard tests/[0123]-*.c)

# the test to run when you call `make run` (must also be in the TEST_SRC line above)
DEFAULT_TEST = 0-sd-test.c

# all the .c files you want to compile
FAT32_DRIVER_SRC = pi-sd.c mbr.c fat32.c fs.c
FAT32_DRIVER_HELPERS_SRC = mbr-helpers.c fat32-helpers.c fat32-lfn-helpers.c
EXTERNAL_SRC = external-code/unicode-utf8.c external-code/bzt-sd.c

# the installer to use (my-install or pi-install)
INSTALL = my-install

# the serial port to use with my-install (leave blank for the default)
INSTALL_SERIAL_PORT =

TEST_OBJS := $(patsubst %.c, %.o, $(TEST_SRC))
TEST_PROGS := $(patsubst %.c, %.bin, $(TEST_SRC))

OBJS := $(FAT32_DRIVER_SRC:.c=.o) $(FAT32_DRIVER_HELPERS_SRC:.c=.o)
EXTERNAL_OBJS := $(EXTERNAL_SRC:.c=.o)
DEFAULT_TEST_BIN := $(DEFAULT_TEST:.c=.bin)

# check that your path is defined.
ifndef CS140E_2021_PATH
$(error CS140E_2021_PATH is not set: this should contain the absolute path to where this directory is.  Define it in your shell's initialiation.  For example, .tcshrc for tcsh or .bashrc for bash)
endif

include $(CS140E_2021_PATH)/libpi/includes.mk
CFLAGS += -Wno-unused-function -Wno-unused-variable -falign-functions=4 
CFLAGS += -Iexternal-code

# add all the .h's as dependencies, libpi and the makefile.
DEPS := $(MEMMAP) $(LPI) $(wildcard ./*.h) ./Makefile 

LIBS = $(LPI)

all: libs $(TEST_PROGS) tags

libs: 
	make -C $(LPP)

$(TEST_OBJS) $(OBJS) $(EXTERNAL_OBJS): $(DEPS)
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@
%.o: %.S
	$(CC) -c $(CPP_ASFLAGS) $< -o $@

%.elf: %.o $(EXTERNAL_OBJS) $(OBJS) $(MEMMAP) $(DEPS) 
	$(LD) $(START) $< $(OBJS) $(EXTERNAL_OBJS) -T $(LPP)/memmap -o $@  $(LIBS)

%.list: %.elf
	$(OD) -D   $*.elf > $*.list
%.bin: %.list
	$(OCP) $*.elf -O binary $*.bin

tags: 
	ctags -R . $(LPP)

clean:
	rm -rf $(NAME) *~ tags *.o *.bin *.elf *.list *.img Makefile.bak  *.d

run: $(DEFAULT_TEST_BIN:.bin=.run)

# %.run rules are "phony"; they perform an action, but they don't create a file
%.run: %.bin
	@# e.g. my-install /dev/ttyUSB0 0-test-sd.bin
	$(INSTALL) $(INSTALL_SERIAL_PORT) $<

# "|" before make dependencies forces them to run in order
.NOTPARALLEL:
runall: | $(TEST_PROGS:.bin=.run)

%.test: %.bin
	@echo "-------------------------"
	@echo "Running <$<>:"
	@$(INSTALL) $(INSTALL_SERIAL_PORT) $< |& grep -e PASS -e PANIC -e TEST

.NOTPARALLEL:
test: | $(TEST_PROGS)
	@echo "*************************"
	@echo "Running all tests..."
	@$(MAKE) -s $(TEST_PROGS:.bin=.test)
	@echo "*************************"

.PHONY: libs all tags clean run runall %.run test %.test

# stop make from deleting these suffix'd files.
.PRECIOUS: %.list %.elf
