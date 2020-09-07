/*

  String C-Library Include File for MVS-OS/390-zOS

  (C)2003 Jason Paul Winter, All Rights Reserved.

*/

#ifndef __STRINGH
#define __STRINGH /* Prevent multiple includes */

#ifndef NULL
#define NULL 0
#endif

#ifndef size_t
#define size_t unsigned int
#endif

void * memchr  (const void *, int, size_t);
//*
int    memcmp  (const void *, const void *, size_t);
int    memicmp (const void *, const void *, size_t);
#define _memicmp memicmp
void * memcpy  (void *, const void *, size_t);
void * memccpy (void *, const void *, int, size_t);
#define _memccpy memccpy

void * memset  (void *, int, size_t);

void   _swab   (char *, char *, int);

//*/
char * strcat  (char *, const char *);
char * strchr  (const char *, int);
int    strcmp  (const char *, const char *);
char * strcpy  (char *, const char *);
size_t strlen  (const char *);
//*
char * strncat (char *, const char *, size_t);
int    strncmp (const char *, const char *, size_t);
char * strncpy (char *, const char *, size_t);
char * strnset (char *, int, size_t);
#define _strnset strnset
#define _strset(s,c) strnset (s, c, 0xFFFFFFFF)

char * strrev (char *);
#define _strrev strrev

//*/
char * strrchr (const char *, int);

void * memmove (void *, const void *, size_t);

size_t strcspn (const char *, const char *);
char * strerror (int);
char * strpbrk (const char *, const char *);
size_t strspn  (const char *, const char *);
char * strspnp (const char *, const char *);
#define _strspnp strspnp
char * strstr  (const char *, const char *);
char * strtok  (char *, const char *);

void   bcopy   (const void *, void *, size_t);
void   bzero   (void *, size_t);

char * strdup  (const char *);
#define _strdup strdup

int    strcasecmp (const char *, const char *);
int    strncasecmp (const char *, const char *, size_t);
#define stricmp   strcasecmp
#define _stricmp  strcasecmp
#define strnicmp  strncasecmp
#define _strnicmp strncasecmp

char * strupr  (char *);
char * strlwr  (char *);
#define _strupr strupr
#define _strlwr strlwr

#endif
