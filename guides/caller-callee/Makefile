SOURCES := $(wildcard ./*.c)
OBJS := $(patsubst %.c, %.o, $(SOURCES)) 
LISTS := $(patsubst %.c, %.list, $(SOURCES)) 
dep := $(OBJS:.o=.d) 

# VPATH := $(sort $(dir $(SRC)))

include $(CS140E_2022_PATH)/libpi/defs.mk


# CFLAGS := $(filter-out -Og,$(CFLAGS))
CFLAGS += -Wno-unused-function -Wno-unused-variable  -O2
CFLAGS :=  -O2 -march=armv6

all: $(LISTS)

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@
%.list: %.o
	$(OD) -d   $*.o > $*.list

clean:
	rm -rf $(NAME) *~ tags *.o *.bin *.elf *.list *.img Makefile.bak  *.d 

.PHONY: libs all tags clean run

# stop make from deleting these suffix'd files.
.PRECIOUS: %.list %.elf %.o
