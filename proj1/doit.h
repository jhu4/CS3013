#ifndef DOIT_H
#define DOIT_H

typedef struct{ //background process
	int id;
	char string[128]; 
}bgp;

int countbackground(bgp background[],int size);
void print_system_data();
void fakeshell();
void printcompleted(bgp background[],int size);


#endif