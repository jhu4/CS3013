#include "share.h"

size_t result = 0;

void do_defaultsize(char** argv){
	char* filename = malloc(strlen(argv[1])+1);
	filename = argv[1];
	char* string = malloc(strlen(argv[2])+1);
	string = argv[2];

	doread(filename,string,1024);
}


void do_costumedsize(char** argv){
	char* filename = malloc(strlen(argv[1])+1);
	filename = argv[1];
	char* string = malloc(strlen(argv[2])+1);
	string = argv[2];
	char* specification = malloc(strlen(argv[3])+1);
	specification = argv[3];
	char* endptr;
	int bytesize;
	
	//if the 3rd argument is mmap do mmap version
	if(strcmp(specification,"mmap")==0){
		dommap(filename,string);
	}
	//do read() version otherwise
	else{
		bytesize = strtol(specification,&endptr,10);
		if(specification==endptr){
			fprintf(stderr, "%s is not a integer\n",argv[3]);
			exit(1);
		}
		doread(filename,string,bytesize);
	}
}

void dommap_parallel(char** argv){
	char* filename = malloc(strlen(argv[1])+1);
	filename = argv[1];
	//update the string to global place
	globalstring = malloc(strlen(argv[2])+1);
	globalstring = argv[2];
	char* threadsnum = malloc(strlen(argv[3]+1));
	threadsnum = argv[3];
	int t_num = atoi(++threadsnum);
	if(t_num==0 || t_num<0 || t_num>16){
		fprintf(stderr, "Invalid threads number\n");
		exit(1);
	}
	threadmsg* threadinfo=malloc(sizeof(threadmsg)*t_num);
	pthread_t* threads=malloc(sizeof(pthread_t)*t_num);

	struct stat statbuf;
	int fd = open(filename,O_RDONLY);
	size_t filesize;
	if(fd<0){
		perror("error opening file\n");
		exit(1);
	}

	if (fstat (fd,&statbuf) < 0){
		perror("error in fstate\n");
		exit(1);
	}

	filecontent=(char*)mmap(NULL,statbuf.st_size,PROT_READ,MAP_SHARED,fd,0);
	if(filecontent==(char*)-1){
		perror("Could not map file\n");
		exit(1);
	}

	close(fd);

	//above is copying file content into process space

	filelength=statbuf.st_size;

	//assgin range to each thread
	breakcontent(threadinfo,t_num);

	//creating threads
	for(int i=0;i<t_num;i++){
		if(pthread_create(&threads[i], NULL,threadroutine, (void*)&threadinfo[i])!=0){
			perror("pthread_create");
        	exit(1);
		}
	}

	//let threads run
	for(int i=0;i<t_num;i++){
		pthread_join(threads[i],NULL);
	}

	// for(int i=0;i<t_num;i++){
	// 	printf("index%d  %d %d\n",i,threadinfo[i].startindex,threadinfo[i].stopindex);
	// }


	if(munmap(filecontent,statbuf.st_size)<0){
		perror("Could not unmap file\n");
		exit(1);
	}

	printf("MMAP_pareallel: File size %ld bytes\n",statbuf.st_size);
	printf("Occurences of the string %s is %ld\n",globalstring,result);

}

void dommap(char* filename, char* string){
	struct stat statbuf;
	int fd = open(filename,O_RDONLY);
	size_t filesize;
	globalstring=malloc(strlen(string)+1);
	globalstring=string;
	// fprintf(stderr,"MMAP\n");
	
	if(fd<0){
		perror("error opening file\n");
		exit(1);
	}

	if (fstat (fd,&statbuf) < 0){
		perror("error in fstate\n");
		exit(1);
	}

	filecontent=(char*)mmap(NULL,statbuf.st_size,PROT_READ,MAP_SHARED,fd,0);
	if(filecontent==(char*)-1){
		perror("Could not map file\n");
		exit(1);
	}
	close(fd);
	filelength=statbuf.st_size;

	//start the normal buffer searching with index 0
	// buffersearch(0,filecontent,0,string);
	new_buffersearch(0,(size_t)statbuf.st_size,filecontent,string);
	// mmap_buffersearch(0,statbuf.st_size);

	if(munmap(filecontent,statbuf.st_size)<0){
		perror("Could not unmap file\n");
		exit(1);
	}
	
	
	printf("MMAP: File size %ld bytes\n",statbuf.st_size);
	printf("Occurences of the string %s is %ld\n",string,result);
}

void doread(char* filename, char* string, int bytesize){
	if(bytesize>8192){
		fprintf(stderr, "Size limitation exceed\n");
		exit(1);
	}
	if(bytesize<1){
		fprintf(stderr, "Invalid byte size\n");
		exit(1);
	}

	int id=0;
	size_t index;
	size_t filesize;
	int fragstringsize;
	char* buffer;
	
	result=0;

	FILE* fd = fopen(filename,"r");
	if(fd==NULL){
		fprintf(stderr, "File open error\n");
		exit(1);
	}
	fseek(fd, 0L, SEEK_END);
	filesize = ftell(fd);
	filelength = filesize;
	fseek(fd, 0L, SEEK_SET);
	
	size_t chunknum = ceil_division(filesize,bytesize);
	buffer = malloc(filesize);

	for(size_t i=0;i<chunknum;i++){
		fread(&buffer[i*bytesize],bytesize,1,fd);
	}

	fclose(fd);
	
	int i;
	// fprintf(stderr, "chunknum%d\n",chunknum);
	for(i=0;i<chunknum-1;i++){
			new_buffersearch(i*bytesize,(i+1)*bytesize,buffer,string);
	}
	new_buffersearch(i*bytesize,filesize,buffer,string);
	

	printf("Read: File size %ld bytes\n",filesize);
	printf("Occurences of the string %s is %ld\n",string,result);
}



void new_buffersearch(size_t startindex,size_t endindex,char* buffer,char* string){
	int stringlen = strlen(string);
	size_t i=startindex;
	while(i<endindex){
		if(buffer[i]==string[0] && i+stringlen<=filelength){
			checkstring(string,i,buffer);
		}
		i++;
	}
}


void checkstring(char* string,size_t startindex,char* buffer){
	int i=0;
	size_t j=startindex;
	while(buffer[j]==string[i]){
		i++;
		j++;
		if(i==strlen(string)){
			sem_wait(&mutex);
			result++;
			sem_post(&mutex);
			return;
		}
	}
}

void breakcontent(threadmsg* threadinfo,size_t numberthreads){
	size_t buffersize=ceil_division(filelength,numberthreads);
	size_t i=0;
	//break the length of the file to sets of startindex and stopindex
	for(i=0;i<numberthreads-1;i++){
		threadinfo[i].startindex=i*buffersize;
		threadinfo[i].stopindex=(i+1)*buffersize;
		// printf("INDEX%ld\t%ld\t%ld\n",i,threadinfo[i].startindex,threadinfo[i].stopindex );
	}
	threadinfo[i].startindex=i*buffersize;
	threadinfo[i].stopindex=filelength;
	// printf("INDEX%ld\t%ld\t%ld\n",i,threadinfo[i].startindex,threadinfo[i].stopindex );
}

void* threadroutine(void* info){
	threadmsg* threadinfo=(threadmsg*) info;
	size_t startindex=threadinfo->startindex;
	size_t stopindex=threadinfo->stopindex;
	new_buffersearch(startindex,stopindex,filecontent,globalstring);
}

size_t ceil_division(size_t numerator, size_t denominater){
	size_t result=1+((numerator-1))/denominater;
	return result;
}
