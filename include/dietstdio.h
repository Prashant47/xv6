/* diet stdio */
#ifndef _DIETSTDIO_H
#define _DIETSTDIO_H
#include <sys/cdefs.h>
#include <sys/types.h>
#include "dietfeatures.h"
#ifdef WANT_THREAD_SAFE
#include <pthread.h>
#endif
#include <stdarg.h>

#ifdef WANT_SMALL_STDIO_BUFS
#define BUFSIZE 128
#else
#define BUFSIZE 2048
#endif
#ifndef EOF
#define EOF (-1)
#endif
typedef struct FILE{
  int fd;
  int flags;
  uint bs;	/* read: bytes in buffer */
  uint bm;	/* position in buffer */
  uint buflen;	/* length of buf */
  char *buf;
  struct FILE *next;	/* for fflush */
  pid_t popen_kludge;
  unsigned char ungetbuf;
  char ungotten;
#ifdef WANT_THREAD_SAFE
  pthread_mutex_t m;
#endif
}FILE;

#define ERRORINDICATOR 1
#define EOFINDICATOR 2
#define BUFINPUT 4
#define BUFLINEWISE 8
#define NOBUF 16
#define STATICBUF 32
#define FDPIPE 64
#define CANREAD 128
#define CANWRITE 256
#define CHECKLINEWISE 512

#define _IONBF 0
#define _IOLBF 1
#define _IOFBF 2

//#include <stdio.h>

/* internal function to flush buffer.
 * However, if next is BUFINPUT and the buffer is an input buffer, it
 * will not be flushed. Vice versa for output */
extern int __fflush4(FILE *stream,int next);
extern size_t fwrite(const void *ptr, size_t size, size_t nmemb,FILE *stream);
extern int __buffered_outs(const char *s,size_t len);
int fflush_unlocked(FILE *stream); 

/* ..scanf */
struct arg_scanf {
  void *data;
  int (*getch)(void*);
  int (*putch)(int,void*);
};

int __v_scanf(struct arg_scanf* fn, const char *format, va_list arg_ptr);

struct arg_printf {
  void *data;
  int (*put)(void*,size_t,void*);
};

int __v_printf(struct arg_printf* fn, const char *format, va_list arg_ptr);
int vprintf(const char *format, va_list ap);

extern FILE *__stdio_root;
extern FILE *stdout;
extern FILE *stdin;

int __fflush_stdin(void);
int __fflush_stdout(void);
int __fflush_stderr(void);
int fflush(FILE *stream);

FILE* __stdio_init_file(int fd,int closeonerror,int mode);
int __stdio_parse_mode(const char *mode) __attribute__((__pure__));
void __stdio_flushall(void);

#ifndef __THREAD_INTERNAL_H__
int __libc_close(int fd);
int __libc_open(const char*fn,int flags,...);
ssize_t __libc_read(int fd,void*buf,size_t len);
ssize_t __libc_write(int fd,const void*buf,size_t len);
#endif

FILE *fopen_unlocked(const char *path, const char *mode) __THROW;
FILE *fdopen_unlocked(int fildes, const char *mode) __THROW;
FILE *freopen_unlocked(const char *path, const char *mode, FILE *stream) __THROW;
int fputc_unlocked(int c, FILE *stream); 

int __stdout_is_tty(void);
int __stdin_is_tty(void);
#endif
