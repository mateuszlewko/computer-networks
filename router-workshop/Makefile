CC=gcc 
CFLAGS=-Wall -Wextra -std=gnu99 -g #-O2

all: router

router: utils.o receiver.o sender.o router.o
router.o: router.c

receiver.o: receiver.c receiver.h
sender.o: sender.c sender.h
utils.o: utils.c utils.h
data.o: data.c data.h

.PHONY: clean distclean

clean: 
	rm -f *.o

distclean:
	rm -f router *.o