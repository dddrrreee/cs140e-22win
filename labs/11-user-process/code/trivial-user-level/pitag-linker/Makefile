NAME = pitag-linker

CC = gcc

USE_STAFF = 1
ifeq ($(USE_STAFF),1)
LIBUNIX_PATH := $(CS140E_STAFF_2022_PATH)/libunix
else
LIBUNIX_PATH := $(CS140E_2022_PATH)/libunix
endif

LIBUNIX := $(LIBUNIX_PATH)/libunix.a
CFLAGS := -Wall -Werror -g -Wno-unused-function -Wno-unused-variable -I. -I$(LIBUNIX_PATH) -g -DRPI_UNIX -I$(LPP) -I../pi-side

# just make all .o's depend on everything.  simple.
SOURCES := $(wildcard ./*.c)
HDRS := $(wildcard ./*.h)
ALL_OBJS := $(patsubst %.c, %.o, $(SOURCES)) 
DEPS := ./Makefile $(LIBUNIX) $(LIBUNIX_PATH)/libunix.h $(HDRS)

all : libs $(NAME) 

# make libunix quietly.  remove the '@' and the '-s' if you want to see what is 
# going on.
libs:
	@make -s -C $(LIBUNIX_PATH)

$(ALL_OBJS): $(DEPS) 
$(NAME): $(ALL_OBJS) $(LIBUNIX) 
	$(CC) $(CFLAGS) $(ALL_OBJS) -o $(NAME) $(LIBUNIX) 


clean :
	rm -f *.o *~ Makefile.bak $(NAME)



.PHONY: clean tags libs run
