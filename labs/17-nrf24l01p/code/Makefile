TTYUSB = 
BOOTLOADER = my-install

# the string to extract for checking
GREP_STR := 'HASH:\|ERROR:\|PANIC:\|PASS:\|TEST:\|NRF:\|SUCCESS:'

# set if you want the code to automatically run after building.
RUN = 0

TEST_SRC := $(wildcard ./tests/*-*.c)
# one way send
TEST_SRC := $(wildcard ./tests/[1]-*.c)
TEST_SRC := $(wildcard ./tests/[0]-*.c)

SRC := nrf-util.c nrf-public.c nrf-driver.c
STAFF_OBJS := staff-nrf-driver.o

LIB := libnrf.a

# so we have division.
LIBS += libgcc.a

all:: tags 

include $(CS140E_2022_PATH)/libpi/mk/Makefile.lib.template

test:
	make -C tests
