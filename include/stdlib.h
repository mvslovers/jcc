/*

  StdLib C-Library Include File for MVS-OS/390-zOS

  (C)2003 Jason Paul Winter, All Rights Reserved.

*/

#ifndef __STDLIBH
#define __STDLIBH /* Prevent multiple includes */

#ifndef ptrdiff_t
#define ptrdiff_t long
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef size_t
#define size_t unsigned int
#endif

typedef struct {
    int quot;
    int rem;
} div_t;

typedef struct {
    long quot;
    long rem;
} ldiv_t;

typedef struct {
    long long quot;
    long long rem;
} lldiv_t;

int    abs      (int);
long   labs     (long);

div_t  div      (int, int);
div_t  udiv     (unsigned int, int);
ldiv_t ldiv     (long, long);

long long llabs (long long);

lldiv_t   lldiv    (long long, long long);

#define MIN(a,b)   (((a)<(b))?(a):(b))
#define _min(a,b)  (((a)<(b))?(a):(b))
#define __min(a,b) (((a)<(b))?(a):(b))

#define MAX(a,b)   (((a)>(b))?(a):(b))
#define _max(a,b)  (((a)>(b))?(a):(b))
#define __max(a,b) (((a)>(b))?(a):(b))

unsigned int  _rotl  (unsigned int, int);
unsigned int  _rotr  (unsigned int, int);
unsigned long _lrotl (unsigned long, int);
unsigned long _lrotr (unsigned long, int);

long   atol     (const char *);
#define atoi atol
long   strtol   (const char *, char **, int);
unsigned long strtoul (const char *, char **, int);

long long atoll (const char *);
#define atoq atoll
long long strtoll (const char *, char **, int);
unsigned long long strtoull (const char *, char **, int);
#define strtoq strtoll
#define strtouq strtoull

double atof     (const char *);
double strtod   (const char *, char **);

char * ecvt     (double, int, int *, int *);
char * fcvt     (double, int, int *, int *);
char * gcvt     (double, int, char *);
#define _ecvt ecvt
#define _fcvt fcvt
#define _gcvt gcvt

char * ltoa (long, char *, int);
#define itoa ltoa
char * _ultoa (unsigned long, char *, int);

char * cuserid  (char *);
char * getlogin (void);

void * bsearch  (const void *, const void *, size_t,   size_t, int (*)(const void *, const void *));
void * lsearch  (const void *, const void *, size_t *, size_t, int (*)(const void *, const void *));
void * lfind    (const void *, const void *, size_t *, size_t, int (*)(const void *, const void *));
void   qsort    (void *, size_t, size_t, int (*)(const void *, const void *));
#define _lfind   lfind
#define _lsearch lsearch

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

void   exit     (int);
void   _exit    (int);
#define abort() exit(8)
int    atexit   (void(*func)(void));

void * alloca     (size_t);
#define _alloca alloca
void   free       (void *);
#define __free31 free
#define __free24 free
size_t _msize     (const void *);
#define malloc_usable_size _msize
void * calloc     (size_t, size_t);
void * malloc     (size_t);
void * __malloc31 (size_t);
void * __malloc24 (size_t);
void * realloc    (void *, size_t);

extern char ** _environ;

char * getenv   (const char *);
int    putenv   (const char *);
#define _putenv putenv

int    tolower  (int c);
int    _tolower (int c);
int    toupper  (int c);
int    _toupper (int c);

#define RAND_MAX 0x7fff

int    rand     (void);
void   srand    (unsigned int seed);

#endif
