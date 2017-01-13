#ifndef MYSTACK_H
#define MYSTACK_H
#define MYSTACK_SIZE 50

typedef struct _mystack{
	char* stack[MYSTACK_SIZE];
	int count;
}mystack;

void stack_init(mystack* stack);
int stack_push(mystack* stack,char* num);
char* stack_pop(mystack* stack);
int stack_size(mystack* stack);
#endif 