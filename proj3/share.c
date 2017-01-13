#include "share.h"

extern time_t starttime;
extern sem_t mutex;
extern int globalnum;


int checkandparse(nodeinfo* node, char* buffer){
	int i= 0;
	int j=0;
	char aha;
	char* temp;
	char** endptr;
	temp=strtok(buffer," ");
	if(temp==NULL || strlen(temp)>1){
		return 1;
	}
	node->alphabet=*temp;
	if((node->alphabet-'A')>25 || (node->alphabet-'A')<0){ 
		return 1;
	}
	temp = strtok(NULL," ");
	if(temp==NULL){
		return 1;
	}
	node->value = (int) strtol(temp,endptr,10);
	if(*endptr==temp){
		return 1;
	}
	if(node->value <-32766 || node->value>32767){ 
		return 1;
	}
	temp = strtok(NULL," ");
	if(temp==NULL){
		return 1;	
	}
	node->exectime = (int) strtol(temp,endptr,10);
	if(*endptr==temp){
		return 1;
	}
	if(node->exectime <0 || node->exectime>32767){
		return 1;
	}
	//parse parent char into an array
	temp=strtok(NULL," ");
	while(temp!=NULL){
		if(*temp=='='){
			temp=strtok(NULL," ");	
			break;
		}
		else if(*temp==node->alphabet){
			fprintf(stderr, "Can't wait for itself\n");
			return 1;
		}
		else if((*temp-'A')>25 || (*temp-'A')<0){
			return 1;
		}
		node->parents[i]=*temp;
		i++;
		temp=strtok(NULL," ");
	}

	stack_init(&node->stack);
	// parse numbers into 
	while(temp!=NULL){
		if(*temp=='+'||*temp=='-'||*temp=='*'||*temp=='/'||*temp=='%'){
			node->operation[j]=*temp;
			j++;
			temp=strtok(NULL," ");
			break;
		}
		stack_push(&node->stack,temp);
		temp=strtok(NULL," ");
	}
	
	while(temp!=NULL){
		node->operation[j]=*temp;
		j++;
		temp=strtok(NULL," ");
	}

	return 0;
}


void createthreads(int size,nodeinfo* info,pthread_t* threads){
	int i;
	for(i=0;i<size;i++){
		if(pthread_create(threads, NULL,threadroutine, (void*)info)!=0){
			perror("pthread_create");
        	exit(1);
		}
		threads++;
		info++;
	}
}

void* threadroutine(void* node){
	nodeinfo* info = (nodeinfo*) node;
	time_t endtime;
	int value;
	begin(info->parentsem,strlen(info->parents));
	value=calc(info);
	time(&endtime);
	printf("Node: %c computed a value of %d after %ld second\n"
		,info->alphabet,value,endtime-starttime);
	end(info->childrensem);
}

int calc(nodeinfo* node){
	time_t current,end;
	time(&current);
	int acc=0;
	char* second;
	char* first;
	char result[25];
	char* endptr;
	int firstint,secondint,resultint;
	int size = strlen(node->operation);

	mystack* stack = &(node->stack);
	char* operation = node->operation;

	if(size==0){
		sleep(node->exectime);
		sem_wait(&mutex);
		globalnum+=node->value;
		sem_post(&mutex);
		return node->value;
	}
	else if(stack_size(stack)-size<1){
		fprintf(stderr, "Node: %c OPERATOR AND NUMBER SIZE DONT MATCH\n",node->alphabet);
		sleep(node->exectime);
		sem_wait(&mutex);
		globalnum+=0;
		sem_post(&mutex);
		return 0;
	}

	while(size!=0){
		second = stack_pop(stack);
		if(*second=='I'){
			secondint=node->alphabet-'A';
		}
		else if(*second=='V'){
			sem_wait(&mutex);
			secondint=globalnum;
			sem_post(&mutex);
		}
		else{
			// secondint=atoi(second);
			errno=0;
			secondint=(int)strtol(second,&endptr,10);
			if(errno==ERANGE || second==endptr){
				fprintf(stderr, "Node: %c INVALID EXECUTION\n",node->alphabet);
				time(&end);
				if(end<(current+node->exectime)){
					sleep(node->exectime+current-end);	
				}
				sem_wait(&mutex);
				globalnum+=0;
				sem_post(&mutex);
				return 0;
			}
		}
		first=stack_pop(stack);
		if(*first=='I'){
			firstint=node->alphabet-'A';
		}
		else if(*first=='V'){
			sem_wait(&mutex);
			firstint=globalnum;
			sem_post(&mutex);
		}
		else{
			// firstint=atoi(first);
			errno=0;
			firstint=(int)strtol(first,&endptr,10);
			if(errno==ERANGE || first==endptr){
				fprintf(stderr, "Node: %c INVALID EXECUTION\n",node->alphabet);
				time(&end);
				if(end<(current+node->exectime)){
					sleep(node->exectime+current-end);	
				}
				sem_wait(&mutex);
				globalnum+=0;
				sem_post(&mutex);
				return 0;
			}
		}

		switch(*operation){
			case '+':
				resultint=firstint+secondint;
				break;
			case '-':
				resultint=firstint-secondint;
				break;
			case '*':
				resultint=firstint*secondint;
				break;
			case '/':
				resultint=firstint/secondint;
				break;
			case '%':
				resultint=firstint%secondint;
				break;
			default:
				fprintf(stderr,"Something wrong with the operator\n");
				break;
		}

		sprintf(result, "%d", resultint);
		stack_push(stack,result);
		operation++;
		size--;
	}

	strcpy(result,stack_pop(stack));
	if(result==NULL){
		fprintf(stderr,"Evaluation format in the config file is not right\n");
		acc=0;
	}
	else{
		acc=atoi(result);
	}
	
	time(&end);
	if(end<(current+node->exectime)){
		sleep(node->exectime+current-end);	
	}

	sem_wait(&mutex);
	globalnum+=acc;
	sem_post(&mutex);

	return acc;
}


void begin(sem_t* parentsem,int size){
	for(int i=0;i<size;i++){
		if(parentsem!=NULL){
			if(sem_wait(parentsem)!=0){
				perror("sem_wait failed");
			}
		}
		parentsem++;
	}
}

void end(sem_t** childrensem){
	for(int i=0;i<26;i++){
		if(childrensem[i]!=NULL){
			if(sem_post(childrensem[i])!=0){
				perror("sem_post failed");	
			}
		}
	}
}

void addsem(nodeinfo* info,int size){
	int i,j;
	for(i=0;i<size;i++){
		for (j=0;j<strlen(info[i].parents);j++)
		{
			//add new sem to its parentsem list
			if(sem_init(&info[i].parentsem[j],0,0)!=0){
				perror("sem_init");
				exit(1);
			}
			//add it to its parent childsem list
			addsem_parent(info,&info[i].parentsem[j],info[i].parents[j],size);
			
		}
	}
} 

void addsem_parent(nodeinfo* info,sem_t* sem,char name,int size){
	int i,j;
	for(i=0;i<=size;i++){
		if(info[i].alphabet==name){
			for(j=0;j<26;j++){
				if(info[i].childrensem[j]==NULL){
					info[i].childrensem[j]=sem;
					break;
				}
			}
		}
	}
}