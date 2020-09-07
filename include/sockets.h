/*

  Sockets C-Library Include File for MVS-OS/390-zOS

  (C)2003 Jason Paul Winter, All Rights Reserved.

*/

#ifndef __socketsh
#define __socketsh /* Prevent multiple includes */

#define u_char  unsigned char
#define u_long  unsigned long
#define u_short unsigned short

#define herc_in_addr jcc_in_addr

typedef long fd_mask;

#define NBBY       8
#ifndef FD_SETSIZE
#define FD_SETSIZE 256
#endif
#define NFDBITS (sizeof(fd_mask) * NBBY) /* bits per mask */
#define howmany(x, y) (((x) + ((y) - 1)) / (y))

typedef struct fd_set {
    fd_mask    fds_bits[howmany(FD_SETSIZE, NFDBITS)];
} fd_set;

#define FD_SET(n, p)   ((p)->fds_bits[(n)/NFDBITS] |= (1 << ((n) % NFDBITS)))
#define FD_CLR(n, p)   ((p)->fds_bits[(n)/NFDBITS] &= ~(1 << ((n) % NFDBITS)))
#define FD_ISSET(n, p) ((p)->fds_bits[(n)/NFDBITS] & (1 << ((n) % NFDBITS)))
#define FD_COPY(f, t)  bcopy(f, t, sizeof(*(f)))
#define FD_ZERO(p)     bzero(p, sizeof(*(p)))

#define AF_INET 2
#define PF_INET AF_INET

#define SOCK_STREAM    1 /* stream socket */
#define SOCK_DGRAM     2 /* datagram socket */
#define SOCK_RAW       3 /* raw-protocol interface */
#define SOCK_RDM       4 /* reliably-delivered message */
#define SOCK_SEQPACKET 5 /* sequenced packet stream */

#define SOMAXCONN 256

#define INADDR_ANY  (u_long)0x00000000
#define INADDR_NONE (u_long)0xFFFFFFFF

#define ntohl(x) (x)
#define ntohs(x) (x)
#define htonl(x) (x)
#define htons(x) (x)
#define NTOHL(x) (x)
#define NTOHS(x) (x)
#define HTONL(x) (x)
#define HTONS(x) (x)

#define SOCKADDR_IN struct sockaddr_in
#define LPSOCKADDR  struct sockaddr *
#define SOCKET      long

#define INVALID_SOCKET (-1)
#define SOCKET_ERROR   (-1)

struct hostaddr {
    char  * h_name;
    char ** h_aliases;   // null-terminated list of pointers to strings.
    int     h_addrtype;  // short on Windows
    int     h_length;    // short in Windows
    char ** h_addr_list; // null-terminated list of pointers to 4 chars.
};

struct hostent {        // Called by another name...
    char  * h_name;
    char ** h_aliases;
    int     h_addrtype;
    int     h_length;
    char ** h_addr_list;
};

struct jcc_in_addr {
    union {
        struct { u_char s_b1,s_b2,s_b3,s_b4; } S_un_b;
        u_long S_addr;
    } S_un;
};

struct in_addr {
    u_long s_addr;
};

struct sockaddr {
    u_char sa_len;
    u_char sa_family;
    char   sa_data [14];
};

struct sockaddr_in {  /* Used to define what the socket should look like/for */
    short   sin_family;
    u_short sin_port;
    struct  in_addr sin_addr;
    char    sin_zero [8];
};

struct linger {
    int onoff;
    int linger;
};

struct clientid {
    int  domain;
    char name [8];
    char subtaskname[8];
    char reserved [20];
};

typedef struct timeval_t { // Added for select
    long sec;
    long usec;
} timeval;

//******  MULTI-ARCH  ******

long socket       (long, long, long);
long bind         (long, struct sockaddr *, long);
long connect      (long, struct sockaddr *, long);
long listen       (long, long);
long accept       (long, struct sockaddr *, long *);
long send         (long, void *, long, long);
long recv         (long, void *, long, long);
long select       (long, fd_set *, fd_set *, fd_set *, void *);
long ioctlsocket  (long, long, long *);
long closesocket  (long);

unsigned long inet_addr (const char *);
int           inet_aton (const char *, struct in_addr *);

long fcntl (long ss, int command, int reqarg);
long gethostid (void);
long gethostname (int namelen, char * name);
long getpeername (long ss, struct sockaddr * name);
long getsockname (long ss, struct sockaddr * name, int * size);
long getsockopt (int optname, int * optval, int optlen);
long ioctl (long ss, int command, int reqarg, int * retarg);
long readsocket (long ss, void * buf, int nbyte);
long readv (long ss, void * iov, int iovcnt);
long recvfrom (long ss, void * buf, int nbyte, int flags, struct sockaddr * name);
long recvmsg (long ss, char * msg, int flags);
long sendmsg (long ss, char * msg, int flags);
long sendto (long ss, void * buf, int nbyte, int flags, struct sockaddr * name);
long setsockopt (long ss, int optname, int * optval, int optlen);
long shutdown (long ss, int how);
long writesocket (long ss, void * buf, int nbyte);
long writev (long ss, void * iov, int iovcnt);
long getclientid (struct clientid * client);
long givesocket (long ss, struct clientid * client);
long takesocket (struct clientid * client, long socrecv);
long selectex (long s, fd_set * r, fd_set * w, fd_set * e, void * t, void * selecb);
long getibmopt (int command, void * buf);

struct hostent * gethostbyname (char * name);
struct hostent * gethostbyaddr (struct hostaddr * ha);

//****** JCC SPECIFIC ******

char * ascii2ebcdic (char * in, long len);
char * ebcdic2ascii (char * in, long len);

long WSAGetLastError (void);
#define HercGetLastError WSAGetLastError

// Easier to use give/take socket...

char * give_task (void);
void   give_easy (long s);
long   take_easy (long s, char * subtask_from);

//****** OS/390 ******

#define FIONBIO        ((long)0x8004A77E)
#define FIONREAD       (0x4004A77F)

#define SIOCGIFADDR    ((long)0xC020A70D)
#define SIOCGIFBRDADDR ((long)0xC020A712)
#define SIOCGIFDSTADDR ((long)0xC020A70F)
#define SIOCGIFCONF    ((long)0xC008A714)
#define SIOCGMONDATA   ((long)0xC018D902)

#define SO_DEBUG       (0x00000001)
#define SO_ACCEPTCONN  (0x00000002)
#define SO_REUSEADDR   (0x00000004)
#define SO_KEEPALIVE   (0x00000008)
#define SO_DONTROUTE   (0x00000010)
#define SO_BROADCAST   (0x00000020)
#define SO_USELOOPBACK (0x00000040)
#define SO_LINGER      (0x00000080)
#define SO_OOBINLINE   (0x00000100)
#define SO_SNDBUF      (0x00001001)
#define SO_RCVBUF      (0x00001002)
#define SO_SNDLOWAT    (0x00001003)
#define SO_RCVLOWAT    (0x00001004)
#define SO_SNDTIMEO    (0x00001005)
#define SO_RCVTIMEO    (0x00001006)
#define SO_ERROR       (0x00001007)
#define SO_TYPE        (0x00001008)

#define MSG_OOB        (1)
#define MSG_PEEK       (2)
#define MSG_DONTROUTE  (4)
#define MSG_KILLROUTE  (8)

#define F_GETFL        (3)
#define F_SETFL        (4)

//******   END  ******

/* Windows programs working in MVS? */
#define WSAEINVAL          EINVAL
#define WSAEWOULDBLOCK     EWOULDBLOCK
#define WSAEINPROGRESS     EINPROGRESS
#define WSAEALREADY        EALREADY
#define WSAENOTSOCK        ENOTSOCK
#define WSAEDESTADDRREQ    EDESTADDRREQ
#define WSAEMSGSIZE        EMSGSIZE
#define WSAEPROTOTYPE      EPROTOTYPE
#define WSAENOPROTOOPT     ENOPROTOOPT
#define WSAEPROTONOSUPPORT EPROTONOSUPPORT
#define WSAESOCKTNOSUPPORT ESOCKTNOSUPPORT
#define WSAEOPNOTSUPP      EOPNOTSUPP
#define WSAEPFNOSUPPORT    EPFNOSUPPORT
#define WSAEAFNOSUPPORT    EAFNOSUPPORT
#define WSAEADDRINUSE      EADDRINUSE
#define WSAEADDRNOTAVAIL   EADDRNOTAVAIL
#define WSAENETDOWN        ENETDOWN
#define WSAENETUNREACH     ENETUNREACH
#define WSAENETRESET       ENETRESET
#define WSAECONNABORTED    ECONNABORTED
#define WSAECONNRESET      ECONNRESET
#define WSAENOBUFS         ENOBUFS
#define WSAEISCONN         EISCONN
#define WSAENOTCONN        ENOTCONN
#define WSAESHUTDOWN       ESHUTDOWN
#define WSAETOOMANYREFS    ETOOMANYREFS
#define WSAETIMEDOUT       ETIMEDOUT
#define WSAECONNREFUSED    ECONNREFUSED
#define WSAELOOP           ELOOP
#define WSAENAMETOOLONG    ENAMETOOLONG
#define WSAEHOSTDOWN       EHOSTDOWN
#define WSAEHOSTUNREACH    EHOSTUNREACH
#define WSAENOTEMPTY       ENOTEMPTY
#define WSAEPROCLIM        EPROCLIM
#define WSAEUSERS          EUSERS
#define WSAEDQUOT          EDQUOT
#define WSAESTALE          ESTALE
#define WSAEREMOTE         EREMOTE

#endif
