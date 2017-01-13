all: proj4 doit

doit: doit.c
	gcc -o doit doit.c
proj4: proj4.c share.c share.h
	gcc -g -std=c99 -o proj4 proj4.c share.c -lpthread
clean:
	rm proj4 doit
