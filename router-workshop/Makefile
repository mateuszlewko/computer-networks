CC=gcc 
CFLAGS=-Wall -Wextra -std=gnu99 -O2 -Werror

all: router

router: data.o receiver.o sender.o router.o utils.h
router.o: router.c

receiver.o: receiver.c receiver.h
sender.o: sender.c sender.h
data.o: data.c data.h

.PHONY: clean distclean

clean: 
	rm -f *.o

distclean:
	rm -f router *.o