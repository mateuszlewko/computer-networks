CC = gcc 
CFLAGS = -Wall -Wextra -std=gnu99 -O2
LDFLAGS = -lpthread

all: transport

transport: window.o receiver.o sender.o transport.o
transport.o: transport.c

receiver.o: receiver.c receiver.h
sender.o: sender.c sender.h
window.o: window.c window.h

.PHONY: clean distclean

clean: 
	rm -f *.o

distclean:
	rm -f transport *.o