#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "x86.h"
#define STRALIGN(x) (((unsigned long)x&7)?8-((unsigned long)x&7):0)
#define UNALIGNED(x,y) (((unsigned long)x & (sizeof (unsigned long)-1)) ^ ((unsigned long)y & (sizeof (unsigned long)-1)))
char *
strncpy(char *dest,char *src, int n)
{
	int i;

	for (i = 0; i < n && src[i] != '\0'; i++)
	   dest[i] = src[i];
	for ( ; i < n; i++)
	   dest[i] = '\0';

	return dest;
}
char*
strcpy(char *s, char *t)
{
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
    ;
  return os;
}

char*
strcat(char *s, char *t)
{
  char *os;
  os = s;
  while(*s++ != 0)
  ;
  s--;
  while((*s++ = *t++) != 0){
  }
  *s = 0;
  return os;
}
int
strcmp(const char *p, const char *q)
{
  while(*p && *p == *q)
    p++, q++;
  if( ( (uchar)*p - (uchar)*q ) == 0 )
	  return 0;
  return ( (( uchar)*p - (uchar)*q ) > 0 ? 1 : -1);
}
int
strncmp(const char *s1, const char *s2,int n)
{/*
  while(*p && *p == *q && n>0)
    p++, q++,n--;
  if( ( (uchar)*p - (uchar)*q ) == 0 )
	  return 0;
  return ( (( uchar)*p - (uchar)*q ) > 0 ? 1 : -1);
  */
  register const unsigned char* a=(const unsigned char*)s1;
  register const unsigned char* b=(const unsigned char*)s2;
  register const unsigned char* fini=a+n;
  while (a!=fini) {
    register int res=*a-*b;
    if (res) return res;
    if (!*a) return 0;
    ++a; ++b;
  }
  return 0;
}

uint
strlen(char *s)
{
  int n;

  for(n = 0; s[n]; n++)
    ;
  return n;
}

void*
memset(void *dst, int c, uint n)
{
  stosb(dst, c, n);
  return dst;
}

char*
strchr(const char *s, char c)
{
  for(; *s; s++)
    if(*s == c)
      return (char*)s;
  return 0;
}

char*
gets(char *buf, int max)
{
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
    cc = read(0, &c, 1);
    if(cc < 1)
      break;
    buf[i++] = c;
    if(c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
  return buf;
}

int
stat(char *n, struct stat *st)
{
  int fd;
  int r;

  fd = open(n, O_RDONLY);
  if(fd < 0)
    return -1;
  r = fstat(fd, st);
  r = st->size;
  close(fd);
  return r;
}

int
atoi(const char *s)
{
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
    n = n*10 + *s++ - '0';
  return n;
}
/*void*
memcpy(void *vdst, void *vsrc, int n){
	return memmove(vdst,vsrc,n);
}*/
void *
memcpy (void *dst, void *src, int n)
{
    void           *res = dst;
    unsigned char  *c1, *c2;
    c1 = (unsigned char *) dst;
    c2 = (unsigned char *) src;
    while (n--) *c1++ = *c2++;
    return (res);
/*
    int             tmp;
    unsigned long  *lx1 = NULL;
    const unsigned long *lx2 = NULL;

    if (!UNALIGNED(dst, src) && n > sizeof(unsigned long)) {

	if ((tmp = STRALIGN(dst))) {
	    c1 = (unsigned char *) dst;
	    c2 = (unsigned char *) src;
	    while (tmp-- && n--)
		*c1++ = *c2++;
	    if (n == (int) - 1)
		return (res);
	    dst = c1;
	    src = c2;
	}

	lx1 = (unsigned long *) dst;
	lx2 = (unsigned long *) src;

	for (; n >= sizeof(unsigned long); n -= sizeof(unsigned long))
	    *lx1++ = *lx2++;
    }

    if (n) {
	c1 = (unsigned char *) (lx1?lx1:dst);
	c2 = (unsigned char *) (lx1?lx2:src);
	while (n--)
	    *c1++ = *c2++;
    }

    return (res);
*/
}
void*
memmove(void *vdst, void *vsrc, int n)
{
  char *dst, *src;
  
  dst = vdst;
  src = vsrc;
  while(n-- > 0)
    *dst++ = *src++;
  return vdst;
}
/*
double
pow(double mant, double expo){
 unsigned int  e;
    long double   ret;

    if ( mant == 0. ) {
        if ( expo > 0. )
            return 0.;
        else if ( expo == 0. )
            return 1.;
        else
            return 1./mant;
    }
    
    if ( expo == (int) (e = (int) expo) ) {
           
        if ( (int)e < 0 ) {
            e    = -e;
            mant = 1./mant;
        }
   
        ret = 1.;
        
        while (1) {
            if ( e & 1 )
                ret *= mant;
            if ( (e >>= 1) == 0 )
                break;
            mant *= mant;
         }
        return ret;
    }
    return exp ( log (mant) * expo );
}*/
