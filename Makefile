CFlags=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

cmplr:$(OBJS)
		$(CC) -o cmplr $(OBJS) $(LDFLAGS)

$(OBJS):cmplr.h

test:cmplr
		./test.sh

clean:
		rm -f cmplr *.o *~ tmp*

.PHONY: test clean