# for rewriting we just do a few programs.  all in this directory.  we use
# the objects in staff-objs.  compile a program both with and without 
# tracing.

NAME = trace-foo
# this is passed to ld
TRACE_FLAGS = --wrap=foo
OBJS    := $(NAME).o foo.o

CC = gcc
CFLAGS = -Wall -Wno-unused-function -Wno-unused-variable 

all: $(OBJS) $(NAME) run

run: ./$(NAME)
	@echo "if --wrap worked: should see calls to both 'wrap_foo' and 'real_foo'";
	./$(NAME)
	
$(OBJS): $(DEPS)
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	$(CC) -Wl,$(TRACE_FLAGS) $(OBJS) -o $@ 

clean:
	rm -rf $(NAME) *~ *.o Makefile.bak  

.PHONY: all clean  run
