CC=gcc 
CFLAGS=-Wall -Wextra -std=gnu99 -O2 -Werror

all: transport

transport: sender.o transport.o
transport.o: transport.c

# receiver.o: receiver.c receiver.h
sender.o: sender.c sender.h
# utils.o: utils.c utils.h

.PHONY: clean distclean

clean: 
	rm -f *.o

distclean:
	rm -f transport *.o