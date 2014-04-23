#include "./include/stdio.h"
#include "types.h"
#include "user.h"
#include "ctype.h"

static FILE _io_stdout = {
	.desc = 1,
};
FILE *stdout = &_io_stdout;
static FILE _io_stdin = {
	.desc = 0,
};
FILE *stdin = &_io_stdin;
static FILE _io_stderr = {
	.desc = 0,
};
FILE *stderr = &_io_stderr;

int checkmode(char *mode, int *seek){
  int f;
  for (;;) {    
    switch (*mode) {
    case 0: return f;
    case 'b': break;
    case 'r': f=O_RDONLY; break;
    case 'w': f=O_WRONLY|O_CREATE; break;
    case 'a': f=O_WRONLY|O_CREATE; *seek = 1;  break;
    case '+': f|=O_RDWR; break;
    }
    ++mode;
  }
}

FILE *fopen(char *path,char* mode){
  int fd ;
  FILE *fp;
  fp  = (FILE *)malloc(sizeof(FILE));
  
  if(fp == (void *)0);
//    printf("malloc problem\n");

  int m,seekset = 0;
  m = checkmode(mode,&seekset );
  fd = open(path, m);

  if(seekset)
    lseek(fd,0,2);

  fp->desc = fd;
  return fp;
}

  

int fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
  if(stream == (void*)0)
    return 0;
  int res;
  /*printf("fd = %d\n",stream->desc );*/
  res = read(stream->desc,ptr,size*nmemb);
  return res;
}
  

int fwrite(void *ptr, size_t size , size_t nmemb, FILE *stream){
  if(stream == (void*)0)
    return 0;
// if STDIO is buffered implementation should be added its simple direct implememtation
  int res;
  res = write(stream->desc, ptr, size*nmemb);
  return res/size;
}
 
 
int fclose(FILE *fp){
	int ret;
  if(fp == (void*)0)
    return 1;
  ret = close(fp->desc);
  free(fp);
  return ret;
 // exit(1);
}
  
int fseek(FILE *stream, int off, int whence){
  return lseek(stream->desc, off, whence);
}

int fputs(char *s,FILE *stream){
	return fwrite(s,strlen(s),1,stream);
}
