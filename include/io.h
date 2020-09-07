/*

  Low Level IO C-Library Include File for MVS-OS/390-zOS

  (C)2003 Jason Paul Winter, All Rights Reserved.

*/

#ifndef __IOH
#define __IOH /* Prevent multiple includes */

#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

#ifndef EOF
 #define EOF (-1)
#endif

#define _O_RDONLY 0x0000
#define _O_WRONLY 0x0001
#define _O_RDWR   0x0002

#define _O_TEXT   0x0004
#define _O_BINARY 0x0008

#define _O_APPEND 0x0010
#define _O_TRUNC  0x0020

#define _O_CREAT  0x0040
#define _O_EXCL   0x0080

#define _O_ATTRS  0x0100 /* Special case for MVS-OS/390-zOS */

#define O_APPEND  _O_APPEND
#define O_BINARY  _O_BINARY
#define O_CREAT   _O_CREAT
#define O_EXCL    _O_EXCL
#define O_RDONLY  _O_RDONLY
#define O_RDWR    _O_RDWR
#define O_TEXT    _O_TEXT
#define O_TRUNC   _O_TRUNC
#define O_WRONLY  _O_WRONLY

#define SEEK_SET  0
#define SEEK_CUR  1
#define SEEK_END  2

extern int _fmode;

int  close   (int h);
int  commit  (int h);
int  dup     (int h);
int  dup2    (int h, int n);
int  eof     (int h);
int  isatty  (int h);
long lseek   (int h, long offset, int origin);

#ifndef __novaargs
 int  open (const char * filename, int oflag, ...);
#else
 int  open (const char * filename, int oflag, int pmode, char * dcb);
#endif

int  read    (int h, char * buf, unsigned int count);
int  setmode (int h, int mode);
long tell    (int h);
int  write   (int h, const char * buf, unsigned int count);
long filelength (int h);

#define _close close
#define _commit commit
#define _dup dup
#define _dup2 dup2
#define _eof eof
#define _isatty isatty
#define _lseek lseek
#define _open open
#define _read read
#define _setmode setmode
#define _tell tell
#define _write write
#define _filelength filelength

int __bldl    (int h);
int __note    (int h);
int __setstow (int h, char * buf, long len, int ttrn);
int __getdcb  (int h, unsigned char  * dsorg,
                      unsigned char  * recfm,
                      unsigned char  * keylen,
                      unsigned short * lrecl,
                      unsigned short * blksize);
void * __getdcba (int h);

int _unlink (const char * path);
int remove  (const char * path);
int rename  (const char * oldname, const char * newname);

int __get_tmpddn (char * name, char * dd9);
int __get_ddndsnmemb (int h, char * ddn, char * dsn,
             char * member, char * serial, unsigned char * flags);

int _truncate (int h, int l); /* NOT FOR DSN or DDN FILES */

#endif
