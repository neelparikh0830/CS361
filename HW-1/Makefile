hw1: $(wildcard *.c)
	@gcc -c -Wall hw1.c
	@gcc -Wall -o hw1 *.c

test: hw1
	@readelf -sW hw1.o

clean:
	rm -f hw1 hw1.o
