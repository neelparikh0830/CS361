LIBS = -L/usr/local/lib

all: gdb strace ltrace

gdb: gdb.c
	gcc -g gdb.c -o trygdb

strace: strace.c
	gcc -g strace.c -o trystrace

ltrace: ltrace.c
	gcc -g ltrace.c -o tryltrace

clean:
	rm trygdb trystrace tryltrace
