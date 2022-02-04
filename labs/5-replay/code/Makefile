PROG_SRC = run-trace.c  run-pi-timeout.c # trace-exec.c # run-interpose.c trace.c # fail-pi.c
SRC = interpose.c

PI_DIR = ../my-install
CFLAGS = -I$(PI_DIR)

LIBS = -lutil

# does not seem to be handling dependencies?
# include $(CS140E_STAFF_2022_PATH)/libunix/mk/Makefile.unix
include $(CS140E_2022_PATH)/libunix/mk/Makefile.unix

run.trace: ./run-trace
	./run-trace ./my-install /dev/ttyUSB0 hello.bin
	./run-trace ./my-install hello.bin

# run: ./run-interpose
# 	./run-interpose -lo 0 -hi 8 ./my-install /dev/ttyUSB0 hello.bin
# 	make check

run: ./run-pi-timeout
	# ./run-pi-fail -lo 3380 -hi 4000 ./my-install /dev/ttyUSB0 hello.bin

    # this should not work
	./run-pi-timeout -lo 1 -hi 80 my-install /dev/ttyUSB0 hello.bin
	# ./run-pi-fail -lo 4 -hi 60 ./my-install /dev/ttyUSB0 hello.bin
