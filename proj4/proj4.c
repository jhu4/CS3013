#include "share.h"


int main(int argc,char** argv){
	// char* buffer = malloc(10000);
	// FILE* fr = fopen("binary","r");
	// FILE* fw = fopen("binaryoutput","w");
	// if(fr<0 || fw<0){
	// 	exit(1);
	// }
	// fread(buffer,1024,1,fr);
	// fwrite(buffer,1024,1,fw);
	// fclose(fr);
	// fclose(fw);
	sem_init(&mutex,0,1);
	
	if(argc==3){
		//do part 1 
		do_defaultsize(argv);
		
	}
	else if(argc==4){
		//do part 2
		if(*argv[3]=='p'){
			dommap_parallel(argv);
		}
		else{
			do_costumedsize(argv);	
		}
		
	}
	else{
		fprintf(stderr,"The arguments # is invalid\n");
	}
	return 0;
}
