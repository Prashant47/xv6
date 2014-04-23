#include "../fcntl.h"

extern FILE *stdout,*stdin,*stderr;
#define size_t int

/*definations for stdout,stdin */
int checkmode(  char *mode, int *seek);
FILE *fopen(char *path,  char* mode);
int fread( void *ptr, size_t size, size_t nmemb, FILE *stream); 
int fwrite( void *ptr, size_t size , size_t nmemb, FILE *stream);
int fclose(FILE *fp);
int fputs(char *s,FILE *stream);
int fprintf(FILE *,char*, ...);
