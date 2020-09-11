/*

  Error C-Library Include File for MVS-OS/390-zOS

  (C)2003 Jason Paul Winter, All Rights Reserved.

*/

#ifndef __ERRH
#define __ERRH /* Prevent multiple includes */

/* file-system */
#define ENOENT          2         /* No such file or directory */
#define EIO             5
#define EBADF           9         /* Bad file number */
#define ENOMEM          12        /* Not enough memory */
#define EACCES          13        /* Permission denied */
#define EEXIST          17        /* File exists */
#define ENOTDIR         20
#define EISDIR          21
#define EINVAL          22        /* Invalid argument */
#define EMFILE          24        /* Too many open files */
#define ENOSPC          28        /* No space left on device */
#define ESPIPE          29        /* Seek not available */
#define EDOM            33        /* Numerical argument out of domain */
#define ERANGE          34        /* Result too large */

/* non-blocking and interrupt i/o */
#define EAGAIN          35        /* Resource temporarily unavailable */
#define EWOULDBLOCK     EAGAIN    /* Operation would block */
#define EINPROGRESS     36        /* Operation now in progress */
#define EALREADY        37        /* Operation already in progress */

/* ipc/network software -- argument errors */
#define ENOTSOCK        38        /* Socket operation on non-socket */
#define EDESTADDRREQ    39        /* Destination address required */
#define EMSGSIZE        40        /* Message too long */
#define EPROTOTYPE      41        /* Protocol wrong type for socket */
#define ENOPROTOOPT     42        /* Protocol not available */
#define EPROTONOSUPPORT 43        /* Protocol not supported */
#define ESOCKTNOSUPPORT 44        /* Socket type not supported */
#define EOPNOTSUPP      45        /* Operation not supported */
#define EPFNOSUPPORT    46        /* Protocol family not supported */
#define EAFNOSUPPORT    47        /* Address family not supported by protocol family */
#define EADDRINUSE      48        /* Address already in use */
#define EADDRNOTAVAIL   49        /* Can't assign requested address */

/* ipc/network software -- operational errors */
#define ENETDOWN        50        /* Network is down */
#define ENETUNREACH     51        /* Network is unreachable */
#define ENETRESET       52        /* Network dropped connection on reset */
#define ECONNABORTED    53        /* Software caused connection abort */
#define ECONNRESET      54        /* Connection reset by peer */
#define ENOBUFS         55        /* No buffer space available */
#define EISCONN         56        /* Socket is already connected */
#define ENOTCONN        57        /* Socket is not connected */
#define ESHUTDOWN       58        /* Can't send after socket shutdown */
#define ETOOMANYREFS    59        /* Too many references: can't splice */
#define ETIMEDOUT       60        /* Operation timed out */
#define ECONNREFUSED    61        /* Connection refused */

/* Other */
#define ELOOP           62        /* Too many levels of symbolic links */
#define ENAMETOOLONG    63        /* File name too long */
#define EHOSTDOWN       64        /* Host is down */
#define EHOSTUNREACH    65        /* No route to host */
#define ENOTEMPTY       66        /* Directory not empty */
#define EPROCLIM        67        /* Too many processes */
#define EUSERS          68        /* Too many users */
#define EDQUOT          69        /* Disc quota exceeded */
#define ESTALE          70        /* Stale NFS file handle */
#define EREMOTE         71        /* Too many levels of remote in path */

extern long athreadid (void);
extern int  __errno;
#define errno (((int*)&__errno)[athreadid()])

#endif
