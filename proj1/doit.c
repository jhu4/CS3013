/* 	doit.c
	@author Jinan Hu
*/
/*
	The fake shell is not powerful as the linux shell. It cannot change directory.
	It has a bunch of limitation such as the length of the command.
	Also, when the fake shell encounter with a command that isn't a command that execve can
	execute, then some problems rise. Moreover, the fake shell has delays to report finished process. 
*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "doit.h"

/**Execute the command if there is a command, if there is not a command,
	behave like a shell until "exit" command is shown up. Any command ending 
	with & will be ran in the background and print out the status of background
**/
int main(int argc, char** argv){
	int pid;
	int i,j;
	char* argv_new[argc];
	if(argc>1){

		argv++;

		if((pid=fork())<0){
			fprintf(stderr, "Fork error\n");
		}
		else if(pid==0){ //child process
				//executa the command
				if(execvp(argv[0],argv)<0){
					fprintf(stderr,"It's not a command\n");
					exit(1);
				}
		}
		else{ //parent 
			wait(0); //wait for the child to finish
			print_system_data();
		}
	}
	else{
		fakeshell();
	}
	return 1;
}

/**Print the system data out
**/
void print_system_data(){
	struct rusage dataobj;
	struct rusage* data=&dataobj;
	struct timeval tvobj;
	struct timeval* tv=&tvobj;
	struct timezone tzobj;
	struct timezone* tz=&tzobj;
	if(getrusage(RUSAGE_CHILDREN,data)==0){
		printf("CPU time used by system: %ld\n",data->ru_utime.tv_usec);
		printf("CPU time used by user: %ld\n",data->ru_stime.tv_usec);
		printf("The elapsed wall-clock for the command to execute: %ld\n",(gettimeofday(tv,tz)==0?tv->tv_usec:-999));
		printf("The number of times the process was preempted involuntarily: %ld\n",data->ru_nivcsw);
		printf("The number of times the process gave up the CPU voluntarily: %ld\n",data->ru_nvcsw);
		printf("The number of major page faults: %ld\n",data->ru_majflt);
		printf("The number of minor page faults: %ld\n",data->ru_minflt);
		printf("The maximum resident set size used: %ld\n",data->ru_maxrss);
	}
}

/**Faking a shell process
**/
void fakeshell(){
	int i=0,j=0,k=0,pid,status;
	char message[129];
	int flag;
	bgp background[20];
	for(i=0;i<20;i++){
		background[i].id=0;
	}
	
	while(1){
		char* exefile[33];
		printf("==>");
		fgets(message,128,stdin);
			if(strlen(message)>1){ //if we have inputs	
				
				//parse a char string into a string of arguments
				exefile[k]=strtok(message," \n");
				while(exefile[k]!=NULL){
					k++;
					exefile[k]=strtok(NULL," \n");
				}	
	
				if(strcmp(exefile[0],"exit")==0){//check special case "exit"
					wait(0);
					if(!countbackground(background,20)) {
						break;
					}
					else{
						printf("Waiting for background processes to finish\n");
					}
				}			
				else if(strcmp(exefile[0],"cd")==0){//check special case "cd" 
					chdir(exefile[1]);
				}
				else if(strcmp(exefile[0],"jobs")==0){//check special case "jobs"
					for(i=0;i<20;i++){
						if(background[i].id!=0){
							printf("id:%d   %s\n",background[i].id,background[i].string);
						}
					}
				}
				else{//normal command cases:
					if(strcmp(exefile[k-1],"&")!=0){//case which a command not ending with &:
						if((pid=fork())<0){
							fprintf(stderr, "Fork error\n");
						}
						else if(pid==0){//child process
								for(i=0;i<=k;i++){
								}
								if(execvp(exefile[0],exefile)<0){
									fprintf(stderr,"It is not a command\n");
									exit(1);
								}
						}
						else{//parent
							wait(0); //wait for the child to finish
							print_system_data();
						}
					}
					else{//case which a command ending with &:
						exefile[k-1]=NULL;
						flag=1;//set flag to true
						if((pid=fork())<0){
							fprintf(stderr, "Fork error\n");
						}
						else if(pid==0){//child process
								for(i=0;i<=k;i++){
								}
								if(execvp(exefile[0],exefile)<0){
									fprintf(stderr,"It is not a command\n");
									flag=0; //set flag to flase
									exit(1);
								}
						}
						else{ //parent process:first print the id and command. 
								//Then check if any background process finished	
							if(flag){ //if it is an executable command
								printf("id: %d %s\n",pid,message);
								for(i=0;i<20;i++){
									if(background[i].id==0){//try to find an empty spot
										background[i].id=pid;//store the id and command into the spot
										strcpy(background[i].string,message);
										break;
									}
								}			
							}
						}
					}
				}
				k=0; //reset k
			}
			printcompleted(background,20);
	}
}


/**Print out and delete the child process that is completed from the list of struct bgp
 *@bgp list of structure where stores the data of all the background process
 *@int size of the list of struct bgp
**/
void printcompleted(bgp background[], int size){
	int i,status,miao;
	for(i=0;i<size;i++){
		miao =waitpid(background[i].id, &status,WNOHANG);
		if(background[i].id!=0 && (miao<0 || background[i].id==miao)){
			print_system_data();
			printf("id: %d %s  completed\n",background[i].id,background[i].string);
			bgp lala;
			background[i]=lala;
			background[i].id=0;
			wait(0);
		}
	}	
}

/** Count how many background process is running
**/
int countbackground(bgp background[],int size){
	int i,j=0;
	for (int i = 0; i < size; ++i){
		if(background[i].id>0) j++;
	}
	return j;
}
