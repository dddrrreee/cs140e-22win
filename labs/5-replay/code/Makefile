PROG_SRC = run-trace.c  run-pi-timeout.c 
SRC = interpose.c

PI_DIR = ../my-install
CFLAGS = -I$(PI_DIR)

LIBS = -lutil
include $(CS140E_2022_PATH)/libunix/mk/Makefile.unix

run.trace: ./run-trace
	./run-trace my-install hello.bin

run.timeout: ./run-pi-timeout
	# ./run-pi-fail -lo 3380 -hi 4000 ./my-install /dev/ttyUSB0 hello.bin

    # this should not work
	./run-pi-timeout -lo 1 -hi 80 my-install hello.bin
	# ./run-pi-fail -lo 4 -hi 60 ./my-install /dev/ttyUSB0 hello.bin
