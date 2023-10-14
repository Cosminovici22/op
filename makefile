.phony: clean run

CC := gcc
CFLAGS := -g -Wall #-Ofast #-Wall #-fshort-enums
SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, $(SRCS))

op: $(OBJS)
	$(CC) $(CFLAGS) -o op $^
	rm -f *.o

%.o: %.c
	$(CC) $(CFLAGS) -c $<

run: op
	./op test
#####################

clean:
	rm -f *.o op
