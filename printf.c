#include "./include/stdio.h"
#include "types.h"
#include "stat.h"
#include "user.h"

static void
putc(int fd, char c)
{
  write(fd, &c, 1);
}
static void
sprintint(char *s, int xx, int base, int sgn, int *curpointer)
{
  static char digits[] = "0123456789ABCDEF";
  char buf[16];
  int i, neg, tmp;
  uint x;

  neg = 0;
  if(sgn && xx < 0){
    neg = 1;
    x = -xx;
  } else {
    x = xx;
  }

  i = 0;
  do{
    buf[i++] = digits[x % base];
  }while((x /= base) != 0);
  if(neg)
    buf[i++] = '-';
  tmp = *curpointer;
  *curpointer += i;
  while(--i >= 0)
    *(s + i+ tmp) = buf[i];
}

static void
printint(int fd, int xx, int base, int sgn , int *cur)
{
  static char digits[] = "0123456789ABCDEF";
  char buf[16];
  int i, neg;
  uint x;

  neg = 0;
  if(sgn && xx < 0){
    neg = 1;
    x = -xx;
  } else {
    x = xx;
  }

  i = 0;
  do{
    buf[i++] = digits[x % base];
  }while((x /= base) != 0);
  if(neg)
    buf[i++] = '-';
  *cur += i;

  while(--i >= 0)
    putc(fd, buf[i]);
}
void 
sputc(char *str,char ch,int *stringPointer){
	*(str + *stringPointer) = ch;
	(*stringPointer)++;
}
int snprintf(char *st,int n,char *fmt,...){
  char *s;
  char str[512];
  int c, i, state;
  uint *ap;
  state = 0;
  /* due to +1 it fetches the second argument*/
  ap = (uint*)(void*)&fmt + 1;
  int stringPointer = 0;
  for(i = 0; fmt[i]; i++){
    c = fmt[i] & 0xff;
    if(state == 0){
      if(c == '%'){
        state = '%';
      } else {
        sputc(str, c,&stringPointer);
      }
    } else if(state == '%'){
      if(c == 'd'){
        sprintint(str, *ap, 10, 1,&stringPointer);
        ap++;
      } else if(c == 'x' || c == 'p'){
        sprintint(str, *ap, 16, 0,&stringPointer);
        ap++;
      } else if(c == 's'){
        s = (char*)*ap;
        ap++;
        if(s == 0)
          s = "(null)";
        while(*s != 0){
          sputc(str, *s,&stringPointer);
          s++;
        }
      } else if(c == 'c'){
        sputc(str, *ap,&stringPointer);
        ap++;
      } else if(c == '%'){
        sputc(str, c,&stringPointer);
      } else {
        // Unknown % sequence.  Print it to draw attention.
        sputc(str, '%', &stringPointer);
        sputc(str, c , &stringPointer);
      }
      state = 0;
    }
  }
  i = 0;
  while(i < n-1 && i < stringPointer){
	*(st+i) = str[i];
	i++;
  }
  *(st+i) = '\0';
  if(stringPointer > n )
	  return stringPointer - n;
  return n; 
}
int
printf(char *fmt, ...)
{
  char *s;
  int c, i, state, stringPointer;
  uint *ap;
  stringPointer = 0;

  state = 0;
  /* due to +1 it fetches the second argument*/
  ap = (uint*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
    c = fmt[i] & 0xff;
    if(state == 0){
      if(c == '%'){
        state = '%';
      } else {
        putc(1, c);
	stringPointer++ ;
      }
    } else if(state == '%'){
      if(c == 'd'){
        printint(1, *ap, 10, 1,&stringPointer);
        ap++;
      } else if(c == 'x' || c == 'p'){
        printint(1, *ap, 16, 0,&stringPointer);
        ap++;
      } else if(c == 's'){
        s = (char*)*ap;
        ap++;
        if(s == 0)
          s = "(null)";
        while(*s != 0){
          putc(1, *s);
          s++;
	  stringPointer++ ;
        }
      } else if(c == 'c'){
        putc(1, *ap);
        ap++;
	stringPointer++ ;
      } else if(c == '%'){
        putc(1, c);
	stringPointer++ ;

      } else {
        // Unknown % sequence.  Print it to draw attention.
        putc(1, '%');
        putc(1, c);
	stringPointer += 2;
      }
      state = 0;
    }
  }
  return stringPointer; 

}
// Print to the given fd. Only understands %d, %x, %p, %s.
int sprintf(char *str,char *fmt, ...){
  char *s;
  int c, i, state;
  uint *ap;
  state = 0;
  /* due to +1 it fetches the second argument*/
  ap = (uint*)(void*)&fmt + 1;
  int stringPointer = 0;
  for(i = 0; fmt[i]; i++){
    c = fmt[i] & 0xff;
    if(state == 0){
      if(c == '%'){
        state = '%';
      } else {
        sputc(str, c,&stringPointer);
      }
    }else if(state == '%'){
      if(c == 'd'){
        sprintint(str, *ap, 10, 1,&stringPointer);
        ap++;
      } else if(c == 'x' || c == 'p'){
        sprintint(str, *ap, 16, 0,&stringPointer);
        ap++;
      } else if(c == 's'){
        s = (char*)*ap;
        ap++;
        if(s == 0)
          s = "(null)";
        while(*s != 0){
          sputc(str, *s,&stringPointer);
          s++;
        }
      } else if(c == 'c'){
        sputc(str, *ap,&stringPointer);
        ap++;
      } else if(c == '%'){
        sputc(str, c,&stringPointer);
      } else {
        // Unknown % sequence.  Print it to draw attention.
        sputc(str, '%', &stringPointer);
        sputc(str, c , &stringPointer);
      }
      state = 0;
    }
  }
  return stringPointer; 
}
int
fprintf(FILE *stream,char *fmt, ...)
{
  char *s;
  int c, i, state, stringPointer;
  uint *ap;
  stringPointer = 0;

  state = 0;
  /* due to +1 it fetches the second argument*/
  ap = (uint*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
    c = fmt[i] & 0xff;
    if(state == 0){
      if(c == '%'){
        state = '%';
      } else {
        putc(stream->desc, c);
	stringPointer++ ;
      }
    } else if(state == '%'){
      if(c == 'd'){
        printint(stream->desc, *ap, 10, 1,&stringPointer);
        ap++;
      } else if(c == 'x' || c == 'p'){
        printint(stream->desc, *ap, 16, 0,&stringPointer);
        ap++;
      } else if(c == 's'){
        s = (char*)*ap;
        ap++;
        if(s == 0)
          s = "(null)";
        while(*s != 0){
          putc(stream->desc, *s);
          s++;
	  stringPointer++ ;
        }
      } else if(c == 'c'){
        putc(stream->desc, *ap);
        ap++;
	stringPointer++ ;
      } else if(c == '%'){
        putc(1, c);
	stringPointer++ ;

      } else {
        // Unknown % sequence.  Print it to draw attention.
        putc(stream->desc, '%');
        putc(stream->desc, c);
	stringPointer += 2;
      }
      state = 0;
    }
  }
  return stringPointer; 

}
