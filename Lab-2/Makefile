lab2: $(wildcard *.c)
	gcc -c -Wall lab2.c
	gcc -Wall -o lab2 *.c

test: lab2
	@readelf -sW lab2.o

clean:
	rm -f lab2 lab2.o