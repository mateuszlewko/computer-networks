CC=gcc 
CFLAGS=-Wall -Wextra -std=gnu99 -O2

all: traceroute

traceroute: utils.o receiver.o sender.o traceroute.o
traceroute.o: traceroute.c

receiver.o: receiver.c receiver.h
sender.o: sender.c sender.h
utils.o: utils.c utils.h

.PHONY: clean distclean

clean: 
	rm -f *.o

distclean:
	rm -f traceroute *.o