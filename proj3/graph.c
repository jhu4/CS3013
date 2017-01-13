#include "share.h"

time_t starttime;
sem_t mutex;
int globalnum=0;

int main(int argc,char** argv){
	FILE* config;
	int i=0;
	char buffer[60];
	nodeinfo info[26];
	pthread_t threads[26];
	time_t endtime;


	time(&starttime);
	sem_init(&mutex,0,1);
	//scan config file
	config = fopen(argv[1],"r");
	if(config==NULL){
		perror("Error opening file");
		return -1;	
	}
	while(fgets(buffer,60,config)!=NULL && i<=25){
		if(checkandparse(&info[i],buffer)){
			fprintf(stderr,"Format in the config file is not right\n");
			return -1;
		}
		i++;
	}
	fclose(config);

	addsem(info,i);
	createthreads(i,info,threads);
	for(int j=0;j<i;j++){
		(void)pthread_join(threads[j], NULL);
	}

	time(&endtime);
	printf("Total computation resulted in a value of %d after %ld seconds\n"
		,globalnum,endtime-starttime);
}
