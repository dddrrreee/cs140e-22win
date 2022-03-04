PROG_SRC = client.c server.c
# SRC = interpose.c

LIBESP_PATH=../libesp
ESP_LIB = $(LIBESP_PATH)/libesp-unix.a

CFLAGS = -I$(LIBESP_PATH) -DRPI_UNIX
LIBS = -lutil $(ESP_LIB)

# should be pulling this out automatically
DEPS += $(ESP_LIB)


ifdef CS140E_STAFF_2022_PATH
include $(CS140E_STAFF_2022_PATH)/libunix/mk/Makefile.unix
else
include $(CS140E_2022_PATH)/libunix/mk/Makefile.unix
endif

libs::
	make -C $(LIBESP_PATH)

# can use the same program: much easier.
crun:
	./client # /dev/ttyUSB1
srun:
	./server # /dev/ttyUSB0

cleanall:
	make -C ./ clean
	rm -f log*.txt

.PHONY: crun srun
