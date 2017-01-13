#include "mystack.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void stack_init(mystack* stack){
	stack->count=0;
}
int stack_push(mystack* stack,char* num){
	if(stack==NULL){
		fprintf(stderr, "Invalid stack");
		return 1;
	}
	if(stack->count==MYSTACK_SIZE){
		fprintf(stderr,"STACK IS FULL\n");
		return 1;
	}
	stack->stack[stack->count]=malloc(sizeof(num)+1);
	strcpy(stack->stack[stack->count],num);
	stack->count++;
	return 0;
}

char* stack_pop(mystack* stack){
	if(stack==NULL){
		fprintf(stderr, "Invalid stack");
		return NULL;
	}
	if(stack->count==0){
		fprintf(stderr,"NOTHING IS IN STACK\n");
		return NULL;
	}
	stack->count--;
	return stack->stack[stack->count];
}

int stack_size(mystack* stack){
	return stack->count;
}

// int main(int argc,char** argv){
// 	mystack stack;
// 	char hello[] = "hello";
// 	char nihao[] = "nihao";
// 	stack_init(&stack);
// 	stack_push(&stack,nihao);
// 	stack_push(&stack,hello);
// 	char* miao = stack_pop(&stack);
// 	printf("%s\n",miao);

// }