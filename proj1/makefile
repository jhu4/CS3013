all:doit 

doit: doit.o doit.h
	gcc -g doit.o -o doit

doit.o:
	gcc -g -c doit.c

clean:
	rm -f *.o doit