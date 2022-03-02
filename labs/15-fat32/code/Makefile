# makefile configuration
# define this if you need to give the device for your pi
TTYUSB = 
# can override the default bootloader
BOOTLOADER = my-install

# the string to extract for checking
GREP_STR := 'HASH:\|ERROR:\|PANIC:\|PASS:\|TEST:'

# set if you want the code to automatically run after building.
RUN = 1
# set if you want the code to automatically check after building.
#CHECK = 1

TEST_SRC := $(wildcard ./tests/[2]-*.c)
TEST_SRC := $(wildcard ./tests/[1]-*.c)
TEST_SRC := $(wildcard ./tests/[0]-*.c)

# a list of all of your object files.
SRC := pi-sd.c mbr.c fat32.c mbr-helpers.c fat32-helpers.c fat32-lfn-helpers.c external-code/unicode-utf8.c external-code/bzt-sd.c

CFLAGS_EXTRA  = -Iexternal-code

# OPT_LEVEL = -O2

SUPPORT_OBJS := $(SRC:.c=.o)

# these are the binaries we give you: remove these when finished.
STAFF_OBJS =  

include $(CS140E_2022_PATH)/libpi/mk/Makefile.template

# not sure if we should fix the makefile to do this for us.
# clean::
# 	rm -f tests/*.list tests/*.o tests/*.elf tests/*~
