CFlags=-std=c11 -g -static

cmplr:cmplr.c

test:cmplr
		./test.sh

clean:
		rm -f cmplr *.o *~ tmp*

.PHONY: test clean