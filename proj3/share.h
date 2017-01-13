#ifndef SHARE_H
#define SHARE_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <errno.h>

#include "mystack.h"

typedef struct nblock{
	sem_t nblocks;
	int sigtime;
}nblock;

typedef struct __info{
	char alphabet;
	int value;
	int exectime;
	char parents[25];
	sem_t* childrensem[25];
	// nblock childrenblock[25];
	// nblock myblock;
	sem_t parentsem[25];
	mystack stack;
	char operation[25];	
}nodeinfo;

int checkandparse(nodeinfo*,char*);
void createthreads(int i,nodeinfo* info, pthread_t* threads);
void* threadroutine(void* argv);
int calc(nodeinfo* node);
void begin(sem_t* parent,int size);
void end(sem_t** children);
void addsem(nodeinfo* info,int size);
void addsem_parent(nodeinfo* info,sem_t* sem,char name,int size);


#endif