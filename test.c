#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "./include/stdio.h"
extern int i;
int main(int argc,char *argv[]){
	/*
//	printf(1,"value of a is %d\n",a);
	char *src = "dhanesh";
	char *buf = (char *)malloc(sizeof(char) * 16);
//	sprintf(buf,"%d %s %x\n",(int)pow(3.4,1.8),"hi",0);
//	printf("%d",snprintf(buf,5,"%s","dhanesh arole"));
//	buf = memset((void *)buf,'\0',sizeof(char) * 10);
//	printf("%s\n",*buf);
//	printf("%d\n",strncmp("dhanesh","dhaRR",5));
	printf("%s\n",(char *)memcpy(buf,src,8));
	FILE *fp; 
	struct stat fileinfo;
	int bytesread;
	stat(argv[1],&fileinfo);
	char *src = (char *)malloc(sizeof(char) *(fileinfo.size + 1));
	fp = fopen(argv[1],"r");
        bytesread = fread(src,1,fileinfo.st_size,fp);	
	printf("%s\n",src);
//	printf("%d\n",fileinfo.st_size);
	printf("%d\n",bytesread);
	*/
	write(1,"hi\n",sizeof("hi\n"));
	exit(1);
}
