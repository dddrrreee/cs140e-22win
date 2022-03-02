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

TEST_SRC := driver.c
SRC := external-code/bzt-sd.c fat32-helpers.c

CFLAGS += -Iexternal-code

CFLAGS_EXTRA  = -Iexternal-code

# OPT_LEVEL = -O2

SUPPORT_OBJS := $(SRC:.c=.o)

# these are the binaries we give you: remove these when finished.
STAFF_OBJS =  

include $(CS140E_2022_PATH)/libpi/mk/Makefile.template

# not sure if we should fix the makefile to do this for us.
# clean::
# 	rm -f tests/*.list tests/*.o tests/*.elf tests/*~
