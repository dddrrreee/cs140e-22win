MAKEFLAGS +=  --no-print-directory

NAME= cmd-watch
CC = gcc
CFLAGS = -Wall -Werror -g  -Og -Wno-unused-function -Wno-unused-variable

ifndef CS140E_2022_PATH
$(error CS140E_2022_PATH not set!)
endif

LPI = $(CS140E_2022_PATH)/libpi
# where lib unix is

PATH_LIBUNIX = $(CS140E_2022_PATH)/libunix
LIBUNIX = $(PATH_LIBUNIX)/libunix.a

CFLAGS += -I$(PATH_LIBUNIX)

# uncomment the tags part of all to get tags.
all : libunix $(NAME) # tags

libunix:
	make -C $(PATH_LIBUNIX)

$(NAME): $(NAME).c $(LIB) ./Makefile
	$(CC) $(CFLAGS) $< -o $@ $(LIBUNIX)

tags:
	ctags *.[ch]

clean :
	rm -f *.o *~ Makefile.bak $(NAME) tags

.PHONY: libunix tags clean
