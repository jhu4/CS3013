#ifndef SHARE_H
#define SHARE_H

#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
typedef struct __msg{
	int id;
	char* fragstring;
	int fragstringlen;
}message;

typedef struct _threadmsg{
	size_t startindex;
	size_t stopindex;
	char* buffer;
	char* string;
}threadmsg;

void do_defaultsize(char** argv);
void do_costumedsize(char** argv);
void dommap_parallel(char** argv);
void dommap(char* filename,char* string);
void doread(char* filename,char* string, int bytesize);

void breakcontent(threadmsg* threadinfo,size_t numberthreads);
size_t ceil_division(size_t numerator, size_t denominater);

void new_buffersearch(size_t startindex,size_t endindex,char* buffer,char* string);
void checkstring(char*string,size_t startindex,char* buffer);

void* threadroutine(void* info);


size_t filelength;
size_t result;
char* filecontent;
char* globalstring;
sem_t mutex;
// threadmsg* threadinfo;
#endif
