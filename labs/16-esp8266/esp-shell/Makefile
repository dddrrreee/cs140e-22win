PROG_SRC = esp-shell.c
# SRC = interpose.c

ifdef CS140E_STAFF_2022_PATH
include $(CS140E_STAFF_2022_PATH)/libunix/mk/Makefile.unix
else
include $(CS140E_2022_PATH)/libunix/mk/Makefile.unix
endif

# can use the same program: much easier.
crun:
	./esp-shell --last 
srun:
	./esp-shell --first 

.PHONY: crun srun
