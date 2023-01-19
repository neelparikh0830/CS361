LIBS = -L/usr/local/lib -lpcap

all: libsess4.so uselib

libsess4.so: sess4.c sess4.h
	gcc -c -Wall -Werror -fpic sess4.c
	gcc -shared -o libsess4.so sess4.o

uselib: uselib.c libsess4.so
	gcc -L$(PWD) uselib.c -o uselib -lsess4

clean:
	rm libsess4.so uselib sess4.o
