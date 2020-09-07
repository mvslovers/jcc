/*

  StdIO C-Library Include File for MVS-OS/390-zOS

  (C)2003 Jason Paul Winter, All Rights Reserved.

*/

#ifndef __STDIOH
#define __STDIOH /* Prevent multiple includes */

#include <io.h>
#include <stdarg.h>

#ifndef NULL
#define NULL 0
#endif

#ifndef size_t
#define size_t unsigned int
#endif

#define fpos_t long

#define FILENAME_MAX 1024

struct _iobuf {
    int magic;
};
typedef struct _iobuf FILE;

extern FILE * stdin;
extern FILE * stdout;
extern FILE * stderr;

FILE  * fdopen  (int, const char *);
#define _fdopen fdopen
int     fileno   (FILE *);
#define _fileno fileno

void    clearerr (FILE *);
int     fclose   (FILE *);
int     fcloseall (void);
#define _fcloseall fcloseall
int     feof     (FILE *);
int     ferror   (FILE *);
int     fflush   (FILE *);
int     _fflushall (void);
#define _flushall _fflushall
#define flushall _fflushall
int     fgetpos  (FILE *, fpos_t *);
FILE  * fopen   (const char *, const char *);
size_t  fread    (void *, size_t, size_t, FILE *);
FILE  * freopen (const char *, const char *, FILE *);
int     fseek    (FILE *, long, int);
int     fsetpos  (FILE *, const fpos_t *);
long    ftell    (FILE *);
size_t  fwrite   (const void *, size_t, size_t, FILE *);

int     _getw    (FILE *);
int     _putw    (int, FILE *);

int     fgetc    (FILE *);
/*int     getchar  (void);*/
char  * gets     (char *);
int     fputc    (int, FILE *);

#define getc fgetc
#define getchar()  fgetc(stdin)
#define putchar(x) fputc(x, stdout)
#define putc fputc
#define _fgetchar()  fgetc(stdin)
#define _fputchar(x) fputc(x, stdout)

int     ungetc   (int, FILE *);
int     puts     (const char *);
int     fputs    (const char *, FILE *);
char  * fgets    (char *, int, FILE *);

void    perror   (const char *);

void    rewind   (FILE *);

void    setbuf   (FILE *, char *);
int     setvbuf  (FILE *, char *, int, size_t);

int     printf   (const char *, ...);
int     fprintf  (FILE *, const char *, ...);
int     sprintf  (char *, const char *, ...);
int     snprintf (char *, size_t, const char *, ...);
#define _snprintf snprintf
int     vfprintf (FILE *, const char *, va_list);
int     vprintf  (const char *, va_list);
int     vsprintf (char *, const char *, va_list);
int     vsnprintf (char *, size_t, const char *, va_list);
#define _vsnprintf vsnprintf

int     scanf    (const char *, ...);
int     fscanf   (FILE *, const char *, ...);
int     sscanf   (const char *, const char *, ...);
int     vfscanf  (FILE *, const char *, va_list arg);

#endif
