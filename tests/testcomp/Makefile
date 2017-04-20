example: example.o other.o
	cc -o example example.o other.o

example.o: example.c example.h

other.o: other.c example.h

example.h: example.c
	cfunctions example.c

clean:
	rm -f example example.o other.o example.h

