/*
 * MVS38j FTP server.  By Jason Paul Winter, 2003.
 *
 * jasonwinter@hotmail.com, use "Subject" wisely - spam gets deleted without being read.
 *
 * Based on the "Dumb As Fudge FTP Daemon" written in Perl.
 *
 */

/*
 # Dumb As Fudge written in Perl:
 #
 # by  James Ensor (ensor@xcf.berkeley.edu)
 # and Ian Turner  (vectro@pipeline.com)
 #
 */

extern long __libc_arch;
extern long __libc_oarch;
extern long __libc_osvm;

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>

#include <sockets.h>

//#define debug_output 1

/*
char * strupr (char * s) {
    long i;

    i = 0;
    while (s [i] = toupper (s [i])) i++;

    return (s);
};
*/

/* #define MAXBUFLEN 4096 */  /* Normal buffer size for FTP - all/most NW cards/drivers support this. */
#define MAXBUFLEN 32768       /* Largest buffer size allowed in Windows */
/* used for socket send/recv max size, buffers are twice as big (plus one for an optional terminator.) */

#define PATHLENGTH 260

#define FAKE_USER  "user"
#define FAKE_GROUP "group"

#define WELCOME_MESSAGE "*** MVS38j FTP Daemon ***"
#define LOGIN_MESSAGE   "You are now logged in."

#define SERVER_PORT 21
#define SERVER_IP   "any"
/* Use- SERVER_IP   "127.0.0.1" to only accept connections from localhost. */

/**** The following is a back-up mechanism for OSs like Windows that don't return information in getsockname. */
#define PASVADDR    "127,0,0,1"  /* PASV style string for hosts that don't return their IP Address. */
/* Use- PASVADDR    "xxx,xxx,xxx,xxx" for the network address for *this* host, used with PASV connections. */
/**** Leaving it as "127,0,0,1" will allow only local-machine FTP clients to use PASV mode transfers. */

static long running = 1;  /* We are running */
static long optimise = 0; /* The largest numbered socket descriptor, for 'select' processing */

static fd_set rbits;
static fd_set wbits;

typedef struct cb_data_tag * cb_data_ptr;
typedef struct cb_data_tag {
    FILE * fh;        /* Will be *one* of FILE or SOCKET */
    long   rcallback; /* Files are always non-blocking, */
    long   wcallback; /* so just signal if an operation is needed. */
    SOCKET FH;        /* Isn't FILE... */
    void * FUNC_DATA; /* Data pointer to the struct below, or NULL */
    void* (*READ_FUNC) (void *, void *);  /* function address to call for reads */
    void* (*WRITE_FUNC) (void *, void *); /* function address to call for writes */
    cb_data_ptr next; /* This is a list structure. */
} cb_data_t;

static cb_data_ptr head = NULL; /* Nothing in the list yet. */

typedef struct data_tag * data_tag_ptr;
typedef struct data_tag {
    long   use_count;  /* May have many references, so only delete the structure when no more exist. */
    SOCKET SOCK;       /* The MASTER-SOCKET which this data belongs to */
    long   DONE;       /* Signal that the MASTER-SOCKET will be used no more. */
    char * READ_BUF;          /* Incomming commands buffer */
    long   READ_BUF_LENGTH;
    char * WRITE_BUF;         /* outgoing responses buffer */
    long   WRITE_BUF_LENGTH;
    long   AUTHED;     /* Has logged in with a valid password (not checked, just set to 1 regardless.) */
    char   TYPE;       /* A-Ascii, I-Binary */
    char   STRU;       /* F-No structure */
    char   MODE;       /* S-Stream */
    struct sockaddr * DATA_CONNECT_ADDR; /* malloc'd area for building connect to client-socket information */
    SOCKET DATA_SOCK;  /* All file related data transfers uses this SOCKET. */
    SOCKET DATA_LISTEN_SOCK; /* PASV mode transfers allow connecting to this SOCKET. */
    FILE * FILE;       /* File transfers read/write to this descriptor. */
    char   DATA_SOCK_STATUS; /* A-Active, C-Connected, F-Failed, L-Listening */
    long   DATA_SOCK_FAILURE_REASON; /* Will be errno */
    char   XFER;       /* File data direction R/W or not set (0) */
    char * DATA_SOCK_BUF;    /* input / output data for file transfers */
    long   DATA_SOCK_BUF_LEN;
    char * CWD;        /* This users current working directory (ie. root:'\' or PDS:'\...\' in MVS) */
} data_t;

/*
# ftp_servlets have:
# control - control connection
# data - data connection
# auth - true or false
# pasv - true or false
# conn_addr - a sockaddr_in to connect to (given by the PORT command) or NULL

# select loop:
# a list
# keys are file descriptors (numbers)
# values are (refs to) : filehandle, data, read_func, write_func (could add accept_func)
# functions are called like this:
# read_func(filehandle, data)
# read_func or write_func can be NULL (won't be selected on)
*/

static long init () {
    FD_ZERO (&rbits); /* Ensure no SOCKETs are set in the 'select' buffers */
    FD_ZERO (&wbits);

    return (0);
};

static void callbacks_delete_f (FILE * fd) {
    data_tag_ptr d;
    cb_data_ptr p;
    cb_data_ptr t;

    p = NULL;
    t = head;
    while (t) {
        if (t->fh == fd) {

#ifdef debug_output
            printf ("Removing File CallBack: %d\n", (int)fd);
#endif

            if (p) {
                p->next = t->next;
            } else {
                head = t->next;
            };
            if (t->FUNC_DATA) {
                d = t->FUNC_DATA;
                d->use_count--;
                if (d->use_count == 0) {

                    if (d->READ_BUF) free (d->READ_BUF);
                    if (d->WRITE_BUF) free (d->WRITE_BUF);
                    if (d->DATA_CONNECT_ADDR) free (d->DATA_CONNECT_ADDR);
                    if (d->DATA_SOCK_BUF) free (d->DATA_SOCK_BUF);
                    if (d->CWD) free (d->CWD);

                    free (d);
                };
            };
            free (t);
            return;
        };
        p = t;
        t = t->next;
    };
};

static void callbacks_delete (SOCKET fd) {
    data_tag_ptr d;
    cb_data_ptr p;
    cb_data_ptr t;

    p = NULL;
    t = head;
    while (t) {
        if (t->FH == fd) {

#ifdef debug_output
            printf ("Removing Socket CallBack: %d\n", fd);
#endif

            if (p) {
                p->next = t->next;
            } else {
                head = t->next;
            };
            if (t->FUNC_DATA) {
                d = t->FUNC_DATA;
                d->use_count--;
                if (d->use_count == 0) {
                    
                    if (d->READ_BUF) free (d->READ_BUF);
                    if (d->WRITE_BUF) free (d->WRITE_BUF);
                    if (d->DATA_CONNECT_ADDR) free (d->DATA_CONNECT_ADDR);
                    if (d->DATA_SOCK_BUF) free (d->DATA_SOCK_BUF);
                    if (d->CWD) free (d->CWD);

                    free (d);
                };
            };
            free (t);
            return;
        };
        p = t;
        t = t->next;
    };
};

static long vec (fd_set * bits, SOCKET fd, long value) { /* value = -1 just returns the current status */

    if (value != -1) {

        if (value != 0) {
            
#ifdef debug_output
            if (&rbits == bits) {
                printf ("Setting Read Socket %d on\n", fd);
            } else {
                printf ("Setting Write Socket %d on\n", fd);
            };
#endif

            FD_SET (fd, bits);

            return (1);

        } else {

#ifdef debug_output
            if (&rbits == bits) {
                printf ("Setting Read Socket %d off\n", fd);
            } else {
                printf ("Setting Write Socket %d off\n", fd);
            };
#endif
            
            FD_CLR (fd, bits);

            return (0);
        };
    };

    return (FD_ISSET (fd, bits));
};

/*
### callback functions
*/

/*
# add a callbacks to the list, or modify existing callbacks
# takes:
# filehandle
# data for callbacks
# function to call when readable
# function to call when writeable
*/
static void set_callbacks_f (FILE * fh, void * func_data, void * read_func, void * write_func) {
    data_tag_ptr d;
    cb_data_ptr cb_data;
    cb_data_ptr s;

    if (fh == NULL) {
        printf ("no file descriptor for fh, can't set callbacks\n");
        running = 0;
        return;
    }

    cb_data = (cb_data_ptr)malloc (sizeof (cb_data_t));

    // register the callback data
    cb_data->fh = fh;
    cb_data->rcallback = (read_func != NULL);
    cb_data->wcallback = (write_func != NULL);

#ifdef debug_output
    if (cb_data->rcallback) {
        printf ("Setting Read File %d on\n", fh);
    } else {
        printf ("Setting Read File %d off\n", fh);
    };

    if (cb_data->wcallback) {
        printf ("Setting Write File %d on\n", fh);
    } else {
        printf ("Setting Write File %d off\n", fh);
    };
#endif

    cb_data->FH = -1;

    cb_data->FUNC_DATA = func_data;
    if (func_data != NULL) {
        d = func_data;
        d->use_count++;
    };
    
    cb_data->READ_FUNC = read_func;
    cb_data->WRITE_FUNC = write_func;

    if (head != NULL) {
        cb_data->next = NULL;
        s = head;
        while (s->next != NULL) s = s->next;
        s->next = cb_data;
    } else {
        cb_data->next = head;
        head = cb_data;
    };
};

static void set_callbacks (SOCKET fh, void * func_data, void * read_func, void * write_func) {
    data_tag_ptr d;
    cb_data_ptr cb_data;
    cb_data_ptr s;

    if (fh < 0) {
        printf ("no file descriptor for fh, can't set callbacks\n");
        running = 0;
        return;
    }

    cb_data = (cb_data_ptr)malloc (sizeof (cb_data_t));

    // register the callback data
    cb_data->fh = NULL;
    cb_data->rcallback = 0;
    cb_data->wcallback = 0;

    cb_data->FH = fh;

    cb_data->FUNC_DATA = func_data;
    if (func_data != NULL) {
        d = func_data;
        d->use_count++;
    };
    
    cb_data->READ_FUNC = read_func;
    cb_data->WRITE_FUNC = write_func;

    if (head != NULL) {
        cb_data->next = NULL;
        s = head;
        while (s->next != NULL) s = s->next;
        s->next = cb_data;
    } else {
        cb_data->next = head;
        head = cb_data;
    };

    // turn on/off the appropriate callback bits
    vec (&rbits, fh, (long)read_func);  /* Set on only if read_func is set */
    vec (&wbits, fh, (long)write_func); /* Set on only if write_func is set */
};

/*
# disable the read callback for the given filehandle
# this can be reversed by enable_read_callback()
# read callbacks are automatically enabled when added/specified by set_callbacks()
# (takes: a filehandle)
*/

static void disable_read_callback_f (FILE * fh) {
    cb_data_ptr s;

    s = head;
    while (s) {
        if (s->fh == fh) {
#ifdef debug_output
            printf ("Setting Read File %d off\n", fh);
#endif
            s->rcallback = 0;
            break;
        };
        s = s->next;
    };
};

static void disable_read_callback (SOCKET fh) {

    vec (&rbits, fh, 0);
};

/*
# enables a read callback disabled by disable_read_callback()
# (takes: a filehandle)
*/
static void enable_read_callback_f (FILE * fh) {
    cb_data_ptr s;

    s = head;
    while (s) {
        if (s->fh == fh) {
#ifdef debug_output
            printf ("Setting Read File %d on\n", fh);
#endif
            s->rcallback = 1;
            break;
        };
        s = s->next;
    };
};
    
static void enable_read_callback (SOCKET fh) {

    vec (&rbits, fh, 1);
};

/*
# disable the write callback for the given filehandle
# this can be reversed by enable_write_callback()
# write callbacks are automatically enabled when added/specified by set_callbacks()
# (takes: a filehandle)
*/
static void disable_write_callback_f (FILE * fh) {
    cb_data_ptr s;

    s = head;
    while (s) {
        if (s->fh == fh) {
#ifdef debug_output
            printf ("Setting Write File %d off\n", fh);
#endif
            s->wcallback = 0;
            break;
        };
        s = s->next;
    };
};

static void disable_write_callback (SOCKET fh) {

    vec (&wbits, fh, 0);
};

/*
# enables a write callback disabled by disable_write_callback()
# (takes: a filehandle)
*/
static void enable_write_callback_f (FILE * fh) {
    cb_data_ptr s;

    s = head;
    while (s) {
        if (s->fh == fh) {
#ifdef debug_output
            printf ("Setting Write File %d on\n", fh);
#endif
            s->wcallback = 1;
            break;
        };
        s = s->next;
    };
};

static void enable_write_callback (SOCKET fh) {

    vec (&wbits, fh, 1);
};

/*
# remove a callback from this list (generally, this will automatically close
# the associated file handle)
# (takes a filehandle)
*/
static void remove_callbacks_f (FILE * fh) {

    if (fh == NULL) return;

#ifdef debug_output
    printf ("Setting Read File %d off\n", fh);
    printf ("Setting Write File %d off\n", fh);
#endif

    callbacks_delete_f (fh);
};

static void remove_callbacks (SOCKET fh) {

    if (fh < 0) return;

    // turn off the appropriate callback bits
    vec (&rbits, fh, 0);
    vec (&wbits, fh, 0);

    callbacks_delete (fh);
};

/*
# call select() and run the appropriate callbacks
*/
static void run_callbacks () {
    fd_set rout;
    fd_set wout;
    cb_data_ptr cb_data;
    cb_data_ptr cb_next;
    SOCKET fd;
    FILE * fh;
    long   i;
    long   j;
    long * s;
    timeval t;

    t.sec = 2; // Wake every 2 seconds
    t.usec = 0; // plus 0 microsecs.

    while (running) {

        cb_data = head;
        while (cb_data) { /* First, check file-processing to-do: */

            fh = cb_data->fh;
            cb_next = cb_data->next; /* In case it's deleted below... */

            if (fh != NULL) { /* Files only */

                /* if this fh is ready for reading/writing and the callback exists */
                if ((cb_data->rcallback) && (cb_data->READ_FUNC)) {

                    (*cb_data->READ_FUNC) ((void *)fh, cb_data->FUNC_DATA); /* call it */

                } else /* Can't have both readers and writers */
                if ((cb_data->wcallback) && (cb_data->WRITE_FUNC)) {

                    (*cb_data->WRITE_FUNC) ((void *)fh, cb_data->FUNC_DATA); /* call it */
                };
            };
        
            cb_data = cb_next;
        };


        rout = rbits; /* Never modify the real ones... */
        wout = wbits; /* 'select' always returns a new array. */

        /* Calculate the MAX.SOCKET number in: i */
        i = ((optimise + 31) / 32) - 1; /* Get word ptr to last 'long' */
        while ((i > 0) &&
               (((long *)&rout) [i] == 0) &&
               (((long *)&wout) [i] == 0)) i--;

        i = ((i + 1) * 32) - 1; /* Highest Socket to check */
        if (i > optimise) i = optimise; /* may be greater than the known value */

        while ((i >= 0) &&
               (FD_ISSET (i, &rout) == 0) &&
               (FD_ISSET (i, &wout) == 0)) i--;

        if (i < 0) i = 0; // Algorithm fix for IBM sockets (descr 0 allowed)

#ifdef debug_output
            printf ("select entered c=%d r=%08X w=%08X\n", i + 1, ((long *)&rout) [0], ((long *)&wout) [0]);
#endif

        if (__libc_osvm == 0) { // z/OS/390 or MVS3.8j?  No - Don't scan CIB chain

            i = select (i + 1, &rout, &wout, NULL, NULL); /* Wait - No timelimit. */

        } else {

            j = 0;
            while ((j == 0) && (running)) {
                
                j = select (i + 1, &rout, &wout, NULL, &t); /* Wait - With a timelimit. */

                if (j == 0) { // Scan the CIB chain for a STOP command
                    s = (*((long **)548));       // 548->ASCB
                    s = ((long **)s) [14];       //  56->CSCB
                    if (s) {
                        if (__libc_oarch) { // z/OS/390 uses an extension
                            s = ((long **)s) [55];   // 220->CSCX
                            if (s) // ...Maybe.
                                s = ((long **)s) [3];//  12->CIB
                        } else
                            s = ((long **)s) [11];   //  44->CIB

                        while (s) {
                            if (((unsigned char *)s) [4] == 0x40) {
                                running = 0; // Found a STOP command queued
                                break;
                            }
                            s = ((long **)s) [0];//   0->NEXT-CIB
                        }
                    }
                }
            }
            i = j;
        }

#ifdef debug_output
            printf ("select woke up s=%d r=%08X w=%08X errno=%d\n", i, ((long *)&rout) [0], ((long *)&wout) [0], errno);
#endif

        /*
        # need to check if callback exists before running it
        # because one callback can remove another
        */

        cb_data = head;
        while (cb_data) {

            fd = cb_data->FH;
            cb_next = cb_data->next; /* In case it's deleted below... */

            if (fd != -1) { /* Sockets only */

                /* if this fd is ready for reading/writing and the callback exists */
                if ((vec (&rout, fd, -1)) && (cb_data->READ_FUNC)) {

                    (*cb_data->READ_FUNC) ((void *)fd, cb_data->FUNC_DATA); /* call it */

                } else /* Can't have both readers and writers */
                if ((vec(&wout, fd, -1)) && (cb_data->WRITE_FUNC)) {

                    (*cb_data->WRITE_FUNC) ((void *)fd, cb_data->FUNC_DATA); /* call it */
                };
            };

            cb_data = cb_next;
        };
    };
};

/*
### general utility functions
*/

/*
# reads a line (up to LF) from the beginning of a string, and returns it
# the line is removed from the string (returns NULL for a line without a linefeed.)
*/
static char * grab_line (char * in, long * len) {
    long   i;
    long   j;
    char * a = NULL;

    if (*len == 0) return (NULL);

    i = 0;
    while (i < *len)
        if (in [i++] == '\n') break;

    if (i < *len) {
        a = (char *)malloc (i + 1);
        memcpy (a, in, i);
        a [i] = 0;
        j = i;

        if (a [--j] == '\n') a [i] = 0;
        if (a [--j] == '\r') a [i] = 0;

        *len -= i;
        memcpy (in, &(in [i]), *len);

    } else if (in [i - 1] == '\n') {

        a = (char *)malloc (i + 1);
        memcpy (a, in, i);
        a [i] = 0;

        if (a [--i] == '\n') a [i] = 0;
        if (a [--i] == '\r') a [i] = 0;

        in [0] = 0;
        *len = 0;
    };

    return (a);
};

/*
### general I/O functions
*/

/*
# read some data onto the end of a string using sysread()
# (takes: filehandle, buffer, optional number of bytes to read)
# (returns number of bytes read, like sysread())
*/
static long read_into_buf_f (FILE * fh, char * buffer) {
    long i;

    i = fread (buffer, 1, MAXBUFLEN, fh);

#ifdef debug_output
/*
    buffer [i] = 0;
    printf ("Read %d, %d bytes: %s\n", (int)fh, i, buffer);
*/
    printf ("Read %d, %d bytes.\n", (int)fh, i);
#endif

    return (i);
};

static long read_into_buf (SOCKET fh, char * buffer) {
    long i;

    i = recv (fh, buffer, MAXBUFLEN, 0);

#ifdef debug_output
    printf ("Recv %d, %d bytes.\n", (int)fh, i);
#endif

    return (i);
};

/*
# write as much data as possible from the beginning of a string
# all data written will be removed from the string
# (takes: filehandle, buffer)
# (returns number of bytes written, like syswrite())
*/
static long write_from_buf_f (FILE * fh, char * buffer, long size) {
    long bytes_written;

    bytes_written = fwrite (buffer, 1, size, fh);

#ifdef debug_output
/*
    buffer [size] = 0;
    printf ("Write %d, %d bytes (of %d bytes:) %s\n", (int)fh, bytes_written, size, buffer);
*/
    printf ("Write %d, %d bytes (of %d bytes.)\n", (int)fh, bytes_written, size);
#endif

    return (bytes_written);
};

static long write_from_buf (SOCKET fh, char * buffer, long size) {
    long bytes_written;
    long newsize;

    if (size > MAXBUFLEN) {
        newsize = MAXBUFLEN;
    } else {
        newsize = size;
    };

    bytes_written = send (fh, buffer, newsize, 0);

#ifdef debug_output
    printf ("Send %d, %d bytes (of %d bytes.)\n", (int)fh, bytes_written, size);
#endif

    if ((bytes_written > 0) && (size - bytes_written > 0)) {
        memcpy (buffer, &(buffer [bytes_written]), size - bytes_written);
    };

    return (bytes_written);
};

/*
### socket utility functions

# make a socket that listens on the given port
# (takes a port number, and an optional ip address (packed))
# (returns a reference to a socket)
*/
static SOCKET listen_sock (short port, long ip) {
    SOCKET      LISTEN_SOCK;
    SOCKADDR_IN Slocal_adx;

    LISTEN_SOCK = socket (PF_INET, SOCK_STREAM, 0);

    if (LISTEN_SOCK != -1) {

        Slocal_adx.sin_family = AF_INET;
        Slocal_adx.sin_addr.s_addr = ip;
        Slocal_adx.sin_port = htons (port);

        if (bind (LISTEN_SOCK, (LPSOCKADDR)&Slocal_adx, sizeof (Slocal_adx)) != -1) {

            if (listen (LISTEN_SOCK, SOMAXCONN) == -1) {
                closesocket (LISTEN_SOCK);
                return (-1);
            };
        } else {
            closesocket (LISTEN_SOCK);
            return (-1);
        };
    } else {
        return (-1);
    };
    
    if (optimise < LISTEN_SOCK) optimise = LISTEN_SOCK;

    return (LISTEN_SOCK);
};

/*
# accept a connection from the given socket
# (takes a reference to a filehandle, like one from listen_sock())
# (returns a reference to a filehandle)
*/
static SOCKET accept_sock (SOCKET listen_sock) {
    SOCKET ACCEPT_SOCK;

    ACCEPT_SOCK = accept (listen_sock, NULL, NULL);

    if (ACCEPT_SOCK == -1) return (-1);

    if (optimise < ACCEPT_SOCK) optimise = ACCEPT_SOCK;

    return (ACCEPT_SOCK);
};

/*
# makes a non-blocking socket
# (takes no args)
*/
static SOCKET nb_sock () {
    SOCKET NB_SOCK;
    long   i = 1;
    
    NB_SOCK = socket (PF_INET, SOCK_STREAM, 0);
    
    if (NB_SOCK != -1) {

        if (optimise < NB_SOCK) optimise = NB_SOCK;

        ioctlsocket (NB_SOCK, FIONBIO, &i);
        return (NB_SOCK);
    };

    return (-1);
};

/*
# makes a non-blocking socket
# and tries to connect it to the given address
# (takes: a sockaddr to connect to)
*/
static SOCKET nb_connect_sock (struct sockaddr * addr, long * i) {
    SOCKET SOCK;

    SOCK = nb_sock();

    if (SOCK != -1)
        if (connect (SOCK, addr, sizeof (struct sockaddr)) == -1) {
            
            *i = HercGetLastError ();
            if ((*i != WSAEWOULDBLOCK) &&
                (*i != WSAEINPROGRESS)) {

                closesocket (SOCK);
                return (-1);
            };
        };
    
    return (SOCK);
};

/*
# gets the IP that a socket is bound to
# (takes a ref to a fh)
*/
static unsigned long sock_ip (SOCKET SOCK) {
    struct sockaddr a;
    struct sockaddr_in * b;
    int i = sizeof (a);

    b = (void *)&a;

    getsockname (SOCK, &a, &i);

    return (b->sin_addr.s_addr);
};

/*
### generic callback
*/

/*
# write data to the buffer for the given filehandle, and wake up the callback for it
# (takes: a filehandle, a buffer, and data to write)
*/
static void generic_write (SOCKET fh, char * buff, char * data) {

    strcpy (buff, data);

    enable_write_callback (fh);
};

/*
# write a line to the buffer for a given filehandle, and wake up the callback for it
# (takes: a filehandle, a buffer, and data to write)
*/
static void generic_write_line (SOCKET fh, char * buff, char * data) {

    strcpy (buff, data);
    strcat (buff, "\r\n");

    enable_write_callback (fh);
};

/*
# checks if the given socket is connected, by trying to reconnect
*/
static long check_sock_connect (SOCKET fh, struct sockaddr * addr) {
    long i;

    if (fh == -1) {
        printf ("check_sock_connect: fh is undefined\n");
        return (0);
    };

    if (addr == NULL) {
        printf ("check_sock_connect: addr is undefined\n");
        return (0);
    };

    i = connect (fh, addr, sizeof (struct sockaddr));

    if (i == 0) return (1);

    i = HercGetLastError ();

    switch (i) {
    case WSAEISCONN :
    case WSAEINPROGRESS :
    case WSAEALREADY :
    case WSAEINVAL : /* Added for windows */
        return (1);
    default :
        return (0);
    };
};

/*
### ftp server stuff
*/

/*
# write a line out the control connection, and enable its callback
*/
static void ctl_write_line (data_tag_ptr data, char * line) {

    generic_write_line (data->SOCK, &(data->WRITE_BUF [data->WRITE_BUF_LENGTH]), line);
    data->WRITE_BUF_LENGTH = strlen (data->WRITE_BUF);
};

/*
# takes a sockaddr and converts it to a comma-separated list of 6 numbers
*/
static char * sockaddr_to_comma_list (SOCKET SOCK) {
    struct sockaddr a;
    struct sockaddr_in * b;
    struct herc_in_addr l;
    int i = sizeof (a);
    char * s;
    unsigned char c;
    unsigned char d;
    unsigned short p;

    b = (void *)&a;

    getsockname (SOCK, &a, &i);

    s = (char *)malloc (30);

    p = ntohs (b->sin_port);
    d = p % 256;
    c = (p - d) / 256; 

    if (b->sin_addr.s_addr) { /* *** NOT ALL IMPLEMENTATIONS EXPORT THE HOST IP-ADDRESS, SO USE PASVADDR *** */
        l.S_un.S_addr = b->sin_addr.s_addr;
        sprintf (s, "%d,%d,%d,%d,%d,%d", l.S_un.S_un_b.s_b1, l.S_un.S_un_b.s_b2, l.S_un.S_un_b.s_b3, l.S_un.S_un_b.s_b4, c, d);
    } else {
        sprintf (s, "%s,%d,%d", PASVADDR, c, d);
    };

    return (s);
};

/*
# takes a comma-separated list of 6 numbers (or something containing it)
# and converts it to a sockaddr
*/
static long comma_list_to_sockaddr (struct sockaddr_in * b, char * str) {
    long i;
    long j;
    long k;
    char tstr [30];
    unsigned short p;
    struct herc_in_addr l;
    long t;

    strcpy (tstr, str);

    b->sin_family = AF_INET;

    i = 0;
    j = 0;
    k = 0;
    while (1) {
        if ((tstr [i] == ',') || (tstr [i] == 0)) {
            tstr [i] = 0;
            t = atol (&(tstr [j]));
            switch (k) {
            case 0:
                l.S_un.S_un_b.s_b1 = (unsigned char)t;
                break;
            case 1:
                l.S_un.S_un_b.s_b2 = (unsigned char)t;
                break;
            case 2:
                l.S_un.S_un_b.s_b3 = (unsigned char)t;
                break;
            case 3:
                l.S_un.S_un_b.s_b4 = (unsigned char)t;
                b->sin_addr.s_addr = l.S_un.S_addr;
                break;
            case 4:
                p = t * 256;
                break;
            case 5:
                p += (short)t;
                b->sin_port = htons (p);
            };

            k++;
            if (str [i] == 0) break;

            i++;
            j = i;
        } else {
            i++;
        };
    };

    if (k == 6) {
        return 1;
    } else {
        return 0;
    };
};

/*
# takes an server response code (like 230)
# and an ftp message (which may be several lines, and may not be empty)
# and returns it as the server should print it
# for example: (this would be a single output:)
# 230-A
# 230-Multi-Line
# 230 Welcome Message
*/
static char * ftp_server_msg (long code, char * message) {
    long   i;
    long   l;
    char * a;
    char * b;
    char * result;
    char   tstr [16];

    if ((message) && (message [0])) {
        i = 0;
        a = message;
        do {
            i++;
            a = strstr (&(a [1]), "\n");
        } while (a);

        i = strlen (message) + (10 * i);

        result = (char *)malloc (i);
        result [0] = 0;

        sprintf (tstr, "%d-", code);
        l = strlen (tstr);

        i = 0;
        a = message;
        do {
            b = strstr (&(a [1]), "\n");

            if (b) {
                strncpy (&(result [i]), tstr, i);
                i += l;
                strncpy (&(result [i]), a, (a - b));
                i += (a - b);
            } else {
                sprintf (tstr, "%d ", code);
                strncpy (&(result [i]), tstr, l);
                i += l;
                strcpy (&(result [i]), a);
                strcat (&(result [i]), "\r\n");
            };

            a = b;
        } while (a);

    } else {
    
        result = (char *)malloc (16);
        sprintf (result, "%d  \r\n", code);
    };

    return (result);
};

/*
# (takes a socket representing the control connection)
*/
static data_tag_ptr new_ftp_data (SOCKET SOCK) {
    char *       s;
    data_tag_ptr data;

    data = malloc (sizeof (data_t));

    data->use_count = 0;

    data->SOCK = SOCK;

    data->DONE = 0;

    data->READ_BUF = (char *)malloc ((MAXBUFLEN * 2) + 1);
    data->READ_BUF [0] = 0;
    data->READ_BUF_LENGTH = 0;

    data->WRITE_BUF = (char *)malloc ((MAXBUFLEN * 2) + 1);
    s = ftp_server_msg (220, WELCOME_MESSAGE);
    strcpy (data->WRITE_BUF, s);
    free (s);
    data->WRITE_BUF_LENGTH = strlen (data->WRITE_BUF);
    
    data->AUTHED = 0;
    data->TYPE = 'A'; /* either "I" (binary) or "A" (ascii) */
    /* STRU is always "F" (no record structure) */
    data->STRU = 'F';
    /* MODE is always "S" (stream) */
    data->MODE = 'S';

    /* ### data sock stuff */

    data->DATA_CONNECT_ADDR = NULL;
    data->DATA_SOCK = -1;          /* the socket used to transfer data */
    data->DATA_LISTEN_SOCK = -1;   /* the socket to wait for a connection on, in PASV mode */
    data->FILE = NULL;             /* the file to read from/write to */
    
    data->DATA_SOCK_STATUS = ' '; /* either " " (none), "L" (listening), "C" (connecting), "F" failed, "A" (active) */
    data->DATA_SOCK_FAILURE_REASON = 0;

    data->XFER = 0;              /* either 0 (no xfer), "R" (reading from disk), "W" (writing to disk) */

    /* two buffers are needed for ascii mode translation */
    data->DATA_SOCK_BUF = (char *)malloc ((MAXBUFLEN * 2) + 1);
    data->DATA_SOCK_BUF [0] = 0; /* buffer near the network connection */
    data->DATA_SOCK_BUF_LEN = 0;
    
    data->CWD = (char *)malloc (PATHLENGTH);
    strcpy (data->CWD, "/");

    return (data);
};

/*
# closes DATA_SOCK, DATA_LISTEN_SOCK, FILE, and removes their callbacks
# also resets XFER, DATA_SOCK_BUF, and DATA_SOCK_STATUS
# (takes an ftp_data hash)
*/
static void reset_data_connection (data_tag_ptr data) {
    
    remove_callbacks_f (data->FILE);
    if (data->FILE) {
#ifdef debug_output
        printf ("Closing down file: %d\n", (int)data->FILE);
#endif
        fclose (data->FILE);
    };
    data->FILE = NULL;

    remove_callbacks (data->DATA_SOCK);

    if (data->DATA_SOCK != -1) {
#ifdef debug_output
        printf ("Closing down socket: %d\n", data->DATA_SOCK);
#endif
        closesocket (data->DATA_SOCK);
    };

    data->DATA_SOCK = -1;
    
    remove_callbacks (data->DATA_LISTEN_SOCK);

    if (data->DATA_LISTEN_SOCK != -1) {
#ifdef debug_output
        printf ("Closing down socket: %d\n", data->DATA_LISTEN_SOCK);
#endif
        closesocket (data->DATA_LISTEN_SOCK);
    }

    data->DATA_LISTEN_SOCK = -1;

    if (data->DATA_CONNECT_ADDR)
        free (data->DATA_CONNECT_ADDR);
    data->DATA_CONNECT_ADDR = NULL;

    data->DATA_SOCK_STATUS = ' ';
    data->DATA_SOCK_FAILURE_REASON = 0;
    data->XFER = 0;
    data->DATA_SOCK_BUF [0] = 0;
    data->DATA_SOCK_BUF_LEN = 0;
};

static void file_wcb (FILE * file, data_tag_ptr data) {
    unsigned long i;
    long   status;
    char   tstr [60];
    char * s;
    char * t;
    char   acrlf [3];

    if (data->DATA_SOCK_BUF_LEN) {

        /* ASCII translation */
        if (data->TYPE == 'A') {

            acrlf [0] = 13;
            acrlf [1] = 10; /* Convert CRLFs into just LFs */
            acrlf [2] = 0;

            t = data->DATA_SOCK_BUF;

            /* SPECIAL CASE: When a CRLF pair is split over two buffers... */
            if (t [data->DATA_SOCK_BUF_LEN - 1] == acrlf [0]) {
                data->DATA_SOCK_BUF_LEN--; /* remove the split control character */
            };

            status = 0;
            ascii2ebcdic (acrlf, 2);
            ascii2ebcdic (t, data->DATA_SOCK_BUF_LEN);

            t [data->DATA_SOCK_BUF_LEN] = 0; /* assume no embedded nulls */
            s = strstr (t, acrlf);           /* any to replace? */
            while (s) {
                s [0] = acrlf [1]; /* make CR - LF, but don't write the second one */
                i = (s - t) + 1;   /* calculate the right length */
                status += write_from_buf_f (file, t, i);
                status++;          /* make up for the missing LF now */

                t = s + 2;         /* just past the two control characters... */
                s = strstr (t, acrlf);
            };
            status += write_from_buf_f (file, t, strlen (t)); /* embedded nulls not allowed! */

        } else {

            status = write_from_buf_f (file, data->DATA_SOCK_BUF, data->DATA_SOCK_BUF_LEN);

        };
        
        if (status != data->DATA_SOCK_BUF_LEN) {
            printf ("error writing to file!\n");
            sprintf (tstr, "425 Data connection failed: %d", errno);
            ctl_write_line (data, tstr);
            reset_data_connection (data);
            return;
        };

        data->DATA_SOCK_BUF_LEN = 0;
    };

    if (data->DATA_SOCK == -1) {
        /* if there's no socket open to get more data from, the transfer is done */
        reset_data_connection (data);
    } else {
        enable_read_callback (data->DATA_SOCK);
        disable_write_callback_f (data->FILE);
    };
};

static void file_rcb (FILE * file, data_tag_ptr data) {
    long status;

    /* don't let the DATA_SOCK_BUF get full */
    if (data->DATA_SOCK_BUF_LEN > MAXBUFLEN) {
        disable_read_callback_f (file);
        return;
    }

    status = read_into_buf_f (file, &(data->DATA_SOCK_BUF [data->DATA_SOCK_BUF_LEN]));

    if (data->DATA_SOCK != -1) {
        enable_write_callback (data->DATA_SOCK);
    };

    /* if there's an EOF (or an error, for now !!!), clean up */
    if (status <= 0) {
        remove_callbacks_f (file);
        if (file) {
#ifdef debug_output
        printf ("Closing down file: %d\n", (int)file);
#endif
            fclose (file);
        };
        data->FILE = NULL;

        if (data->DATA_SOCK != -1) {
            data->XFER = 0;
        };

        return;
    };

    /* ASCII translation */
    if (data->TYPE == 'A')
        ebcdic2ascii (&(data->DATA_SOCK_BUF [data->DATA_SOCK_BUF_LEN]), status);

    data->DATA_SOCK_BUF_LEN += status;
};

/*
# writes data out on the data connection from a buffer
*/
static void data_sock_wcb (SOCKET data_sock, data_tag_ptr data) {
    long status;
    char tstr [60];
    
    if (data->DATA_SOCK_BUF_LEN) {
        status = write_from_buf (data_sock, data->DATA_SOCK_BUF, data->DATA_SOCK_BUF_LEN);

        if (status <= 0) {
            printf ("error writing to data connection!\n");
            sprintf (tstr, "425 Data connection failed: %d", HercGetLastError ());
            ctl_write_line (data, tstr);
            reset_data_connection (data);
            return;
        };

        data->DATA_SOCK_BUF_LEN -= status;
    };

    /* if there is a file, enable its callback
       (it may be disabled because DATA_SOCK_BUF is full) */
    if (data->FILE) {
        enable_read_callback_f (data->FILE);
    };

    if (data->DATA_SOCK_BUF_LEN == 0) {
        if (data->FILE == NULL) {
            /* if there's no file open to get more data from, the transfer is done */
            ctl_write_line (data, "226 Transfer complete!");
            reset_data_connection (data);
        } else {
            disable_write_callback (data_sock);
        };
    };
};

/*
# read data from the data connection
*/
static void data_sock_rcb (SOCKET data_sock, data_tag_ptr data) {
    long status;
    
    status = read_into_buf (data_sock, &(data->DATA_SOCK_BUF [data->DATA_SOCK_BUF_LEN]));

    /* wake up the disk writer */
    if (data->FILE) {
        enable_write_callback_f (data->FILE);
    };

    /* if there's an EOF (or an error, for now !!!), signal that the transfer is completed */
    if (status <= 0) {
        ctl_write_line (data, "226 Transfer complete.");
        remove_callbacks (data_sock);

        if (data->DATA_SOCK != -1) {
#ifdef debug_output
            printf ("Closing down socket: %d\n", data->DATA_SOCK);
#endif
            closesocket (data->DATA_SOCK);
        };

        data->DATA_SOCK = -1;

        data->XFER = 0;
        
        return;
    };

    data->DATA_SOCK_BUF_LEN += status;
    
    /* if there's too much data, wait for it to thin out */
    if (data->DATA_SOCK_BUF_LEN > MAXBUFLEN) {
        disable_read_callback (data_sock);
    };
};

/*
# sets a callback for the data connection, depending on the transfer status
# only sets a callback if there is an active data sock
*/
static void set_xfer_callback (data_tag_ptr data) {
    
    if (data->DATA_SOCK_STATUS == 'A') {
        if (data->DATA_SOCK == -1) return;

        if (data->XFER == 'R') {
            /* reading from disk means writing to the network */
            set_callbacks (data->DATA_SOCK, data, NULL, &data_sock_wcb);
        } else if (data->XFER == 'W') {
            /* writing to disk means reading from the network */
            set_callbacks (data->DATA_SOCK, data, &data_sock_rcb, NULL);
        } else {
            /* if not yet transferring, just wait
               and the callback will be put in once the connection is made */
            remove_callbacks (data->DATA_SOCK);
        };
    };
};

/*
# helper for the two functions below
*/
static void prepare_data_sock (data_tag_ptr data) {
    long data_conn_status;
    long i;
    char tstr [60];

    if (data->DATA_SOCK_STATUS == 'C') {
        data_conn_status = check_sock_connect (data->DATA_SOCK, data->DATA_CONNECT_ADDR);
    } else {
        if (data->DATA_SOCK != -1) {
            data_conn_status = 1;
        } else {
            data_conn_status = 0;
        };
    };

    /* !!! reimpliment separately for listening and connecting socket */
    if (!data_conn_status) {
        printf ("connect failed.\n");
    
        /* on failure, either respond immediately if a transfer is pending
           or queue up the error */
        if (data->XFER) {
            sprintf (tstr, "425 Can't open data connection: %d", HercGetLastError ());
            generic_write_line (data->SOCK, &(data->WRITE_BUF [data->WRITE_BUF_LENGTH]), tstr);
            data->WRITE_BUF_LENGTH = strlen (data->WRITE_BUF);
            reset_data_connection (data);
        } else {
            i = HercGetLastError ();
            printf ("queueing failure for later reference: %d\n", i);
            reset_data_connection (data);
            data->DATA_SOCK_STATUS = 'F';
            data->DATA_SOCK_FAILURE_REASON = i;
        };

    } else {
        /* if the connection succeeded, set DATA_SOCK_STATUS to active */
        data->DATA_SOCK_STATUS = 'A';
        /* don't need to send anything to the control connection
           it always has something appropriate to say when a transfer starts */
        set_xfer_callback (data);
    };
};

/*
# accepts incoming connections on DATA_LISTEN_SOCK
*/
static void data_listen_sock_rcb (SOCKET data_listen_sock, data_tag_ptr data) {

    data->DATA_SOCK = accept_sock (data_listen_sock);

    /* check if sock accepted properly */

    if (data->DATA_SOCK == -1) {
        printf ("error accepting data listen sock!\n");
    } else {
        prepare_data_sock (data);
    }

    /* !!! set callbacks */
    
    remove_callbacks (data_listen_sock);

    if (data->DATA_LISTEN_SOCK != -1) {
#ifdef debug_output
        printf ("Closing down socket: %d\n", data->DATA_LISTEN_SOCK);
#endif
        closesocket (data->DATA_LISTEN_SOCK);
    };

    data->DATA_LISTEN_SOCK = -1;
};

/*
# waits for DATA_SOCK to connect
*/
static void data_connect_sock_wcb (SOCKET data_connect_sock, data_tag_ptr data) {
    
    /* !!! check if connection successful */

    prepare_data_sock (data);
};

static void ctl_write_msg (data_tag_ptr data, long code, char * message) {
    char * to_write;

    to_write = ftp_server_msg (code, message);

    generic_write (data->SOCK, &(data->WRITE_BUF [data->WRITE_BUF_LENGTH]), to_write);
    data->WRITE_BUF_LENGTH = strlen (data->WRITE_BUF);

    free (to_write);
};

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

#include "mvsdirs.h"

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

/*
# takes a path name, and removes any extra slashes and trailing slashes, 
# and resolves . and ..
*/
static char * canonical_path (char * path) {
    char * a;
    char * b;
    char * c;
    char * result;
    long   i;

    result = (char *)malloc (PATHLENGTH);

    c = path;
    while ((c [0] == '\\') || (c [0] == '/'))
        c++; /* Must start with a (back)slash */

    result [0] = 0;

    a = strstr (c, "\\");
    b = strstr (c, "/");
    if ((a == NULL) ||
        ((b != NULL) && (b < a))) {
        a = b;
    };

    while (a) {
        a [0] = 0;
        if (c [0]) {
            if (strcmp (c, ".") != 0) {

                if (strcmp (c, "..") == 0) {
                    i = strlen (result);
                    while (i) {
                        if (result [--i] == '/') {
                            result [i] = 0;
                            break;
                        };
                    };
                } else {
                    strcat (result, "/");
                    strcat (result, c);
                };
            };
            c = a;
        };
        c++;

        a = strstr (c, "\\");
        b = strstr (c, "/");
        if ((a == NULL) ||
            ((b != NULL) && (b < a))) {
            a = b;
        };
    };

    if (strcmp (c, "..") == 0) {
        i = strlen (result);
        while (i) {
            if (result [--i] == '/') break;
        };
        result [i++] = '/';
        result [i] = 0;
    } else {
        strcat (result, "/");
        if (strcmp (c, ".") != 0) strcat (result, c);
        if ((c [0]) && (strcmp (c, ".") != 0))
            strcat (result, "/");
    };

    free (path);

    return (result);
};

/*
# takes two args, cwd and cd_into
# treats cwd as the current directory, and changes it to the result
# of cd'ing from $cwd into $cd_into
# always returns a canonical path (no .'s, ..'s, or extra slashes)
# if cd_into starts with a /, just gives cd_into
# if cd_into is undefined, just gives cwd
*/
static char * dir_concat (char * cwd, char * cd_into) {
    char * result;

    result = (char *)malloc(PATHLENGTH);

    if (cd_into == NULL) {
        strcpy (result, cwd);
        return (result);

    } else if ((cd_into [0] == '/') ||
               (cd_into [0] == '\\')) {
        strcpy (result, cd_into);

    } else {
        sprintf (result, "%s%s", cwd, cd_into);
    };

    return (canonical_path (result));
};

// New routine to refresh the file-list on an MVS system
static void refresh () {
    p_root r;
    p_root d;
    int i;
    int j;

    numdevs = 0;
    i = dirsupport (); // Re-read the VATLST00 file (not really needed)
    if (i == 0) {
        r = (p_root)malloc (sizeof (t_root));
        if (r == NULL) {
            printf ("Out of memory refreshing, vtoc stage\n");
            return;
        }
        r->dircount = 0;
        r->next = NULL;
        j = 0;
        while (j < numdevs) {
            i += readvtoc (devices [j], units [j], r);
            j++;
        }
    }
    if (i == 0) {
        d = root;
        root = r; // Update the real root
        while (d) { // Clean-up old contents
            r = d;
            d = d->next;
            free (r);
        }
    }
}

/*
# takes an ftp_data, and a directory
# and tries to CD into the given directory
# returns 1 on success, 
# and a -1 if the directory doesn't exist 
# or -2 if it's not really a directory
*/
static long ftp_chdir (data_tag_ptr data, char * cd_into) {
    char * new_cwd;
    char * s;
    char   real_new_cwd [PATHLENGTH];
    FILE * fh;
    long   org;

    new_cwd = dir_concat (data->CWD, cd_into);

    if (strcmp (new_cwd, "/") == 0) {
        strcpy (data->CWD, new_cwd);
        free (new_cwd);
        return (1);
    };

    strcpy (real_new_cwd, &(new_cwd [1])); /* remove slash */
    s = strstr (real_new_cwd, "/");
    s [0] = 0;

    org = getorg (real_new_cwd);
    if (org == 0) { // File wasn't found
        refresh ();
        org = getorg (real_new_cwd); // try again now
    }

    if (org == 2) {
        strcpy (data->CWD, new_cwd);
        free (new_cwd);
        return (1);
    };

    free (new_cwd);

    if (org == 1) {
        return (-2);
    };

    return (-1);
};

/*
# writes a message, based on the status of the data connection
# if the data connection is viable (listening, connecting, or active)
#    returns 1
# otherwise returns 0, and resets the data connection
*/
static long prepare_for_xfer (data_tag_ptr data) {
    char status;
    char tstr [40];

    status = data->DATA_SOCK_STATUS;

    if (status == 'A') {
        ctl_write_line (data, "125 Data connection open, starting transfer");
        return (1);

    } else if ((status == 'L') || (status == 'C')) {
        ctl_write_line (data, "150 Now opening data connection");
        return (1);

    } else {
        if (status == 'F') {
            sprintf (tstr, "425 Data connection failed: %d", data->DATA_SOCK_FAILURE_REASON);
            ctl_write_line (data, tstr);
        } else {
            ctl_write_line (data, "425 No data connection established.");
        };
        reset_data_connection (data);
        return (0);
    };
};

static char * mystrcat (char * dest, char * src, long * clen, long * j) { /* j is always the 'end' of dest */
    long i;

    i = strlen (src);

    if (*j + i >= *clen) {
        *clen = *clen * 2; /* Just keep doubling the buffer until it all fits */
        dest = (char *)realloc (dest, *clen + 1);
    };

    strcpy (&(dest [*j]), src);

    *j += i;

    return (dest);
};

/* Special (Non-PDS) DD names that can be accessed within FTPD */
/* This is primarily intended for DD INTRDR to be added to the FTP PROC */
#define added_ddl 10 // Allow up to 10 user-defined DDs
int  added_ddc = 0;
char added_dds [added_ddl][9];

/*
# returns a listing for the given file or directory, like `ls -l`
-rw-r--r--    1 Administ None         5846 May 14 18:34 w32ctca.o
-rwxrwxrwx    1 Administ None        23407 Jan  8 12:46 xstore.c
-rw-r--r--    1 Administ None        27158 May 14 18:37 xstore.o
-rwxrwxrwx    1 Administ Users        3509 Aug  5  2002 znew
*/
static char * ls_dash_l (char * target) {
    long i;
    long j; /* current length of result */
    long clen;
    char * s;
    char * result;
    char line [PATHLENGTH];
    char name [PATHLENGTH];
    long size = 0;
    long type;
    long stat [2];

    char fmtdate [80];
	time_t ltime;
	struct tm * today;

    p_root r;

    clen = MAXBUFLEN * 2;
    result = (char *)malloc ((MAXBUFLEN * 2) + 1);

    sprintf (result, "total %05d\r\n", 0); /* Make space for 'total' */
    j = strlen (result);

    stat [0] = 1;
    stat [1] = 1024;

    if (strcmp (target, "/") == 0) { /* Root */

        if (added_ddc) { // If special DDs are in use, set up a dummy date
            time (&ltime); // Use NOW as the date.
            today = localtime (&ltime);
            strftime (fmtdate, 40, "%b %d  %Y", today);
            i = 0;
            while (i < added_ddc) { // Add the user DD list to the output
                size += stat[1];
                type = '-';
                sprintf (line, "%cr-xr-xr-x %3d %-8s %-8s %8d %12s %s\r\n",
                           type, stat[0], FAKE_USER, FAKE_GROUP, stat[1], fmtdate, added_dds [i]);

                result = mystrcat (result, line, &clen, &j);
                i++;
            }
        }

        refresh (); // Comment-out for faster directory refreshes (using the cache)
        // Then, to update the listing (if required) use the "REFR" command or ftpcmd.c

        r = root; // Start point
        while (r) {
            i = 0;
            while (i < r->dircount) {
                size += stat[1];

                if (r->directories [i].org == 1) {
                    type = '-';
                } else {
                    type = 'd';
                }

                ltime = r->directories [i].date;
                today = localtime (&ltime);
                strftime (fmtdate, 40, "%b %d  %Y", today);

                sprintf (line, "%cr-xr-xr-x %3d %-8s %-8s %8d %12s %s\r\n",
                           type, stat[0], FAKE_USER, FAKE_GROUP, stat[1], fmtdate, r->directories [i].name);

                result = mystrcat (result, line, &clen, &j);

                i++;
            }
            r = r->next;
        }
    } else {                          /* PDS */
    	
        strcpy (line, &(target [1]));
        s = strstr (line, "/");
        s [0] = 0;
        strupr (line);

        sprintf (name, "//DSN:%s", line);
        readdir (name); /* Go get 'em */

        r = root; // Start point, search for the right date to transmit
        while (r) {
            i = 0;
            while (i < r->dircount) {
                if (stricmp (r->directories [i].name, line) == 0) {
                    ltime = r->directories [i].date;
                    today = localtime (&ltime);
                    strftime (fmtdate, 40, "%b %d  %Y", today);
                    break;
                }
                i++;
            }
            if (i < r->dircount)
                break;
            r = r->next;
        }
        if (!r) { // Couldn't find the right date...
            time (&ltime); // Use NOW as the date.
	        today = localtime (&ltime);
            strftime (fmtdate, 40, "%b %d  %Y", today);
        }

        i = 0;
        while (i < pdsecount) {
            size += stat[1];

            sprintf (line, "-r-xr-xr-x %3d %-8s %-8s %8d %12s %s\r\n",
                       stat[0], FAKE_USER, FAKE_GROUP, stat[1], fmtdate, pdsename [i]);

            result = mystrcat (result, line, &clen, &j);

            i++;
        };
    };

    sprintf (line, "total %05d\r\n", size / 1024); /* Max: 1000 => '01000' */
    i = strlen (line);
    strncpy (result, line, i);

    return (result);
};

/*
# returns a listing for the given directory
*/
static char * ftp_ls (data_tag_ptr data) {

    return ls_dash_l (data->CWD);
};

static long get_cmd_args (char * line, char * cmd, char * args) {
    char * a;

    if ((line == NULL) || (line [0] == 0)) return (0);

    a = strstr (line, " ");
    if (a) {
        a [0] = 0;
        strcpy (cmd, line);
        a [0] = ' ';
        a++;
        strcpy (args, a);
    } else {
        strncpy (cmd, line, 39);
        cmd [39] = 0;
        args [0] = 0;
    };
    
    strupr (cmd);

    return (1);
};


/*
# read in new data on the control connection, and handle any pending commands
*/
static void ftp_sock_rcb (SOCKET sock, data_tag_ptr data) {
    long   status;
    struct sockaddr_in addr;
    long   i;
    long   j;
    char * line;
    char * s;
    char * comma_list;
    char   cmd [40];
    char   args [PATHLENGTH];
    char   tstr [PATHLENGTH];
    char   real_path [PATHLENGTH];
    char * tpath;
    char * tdata;
    char   type;
    FILE * fh;
    SOCKET tsock;
    char   fmode [3];
    int    org;

    /* get whatever data is ready */
    status = read_into_buf (sock, &(data->READ_BUF [data->READ_BUF_LENGTH]));

    /* close the connection on error */
    if (status <= 0) {
        printf ("closing connection due to unexpected EOF\n");
        reset_data_connection (data);
        
        tsock = data->SOCK;

        remove_callbacks (data->SOCK); /* Should delete 'data' */

        if (tsock != -1) {
#ifdef debug_output
            printf ("Closing down socket: %d\n", tsock);
#endif
            closesocket (tsock);
        };

        return;
    }
    
    ascii2ebcdic (&(data->READ_BUF [data->READ_BUF_LENGTH]), status);

#ifdef debug_output
    data->READ_BUF [data->READ_BUF_LENGTH + status] = 0;
    printf ("Recv %d bytes: %s\n", status, &(data->READ_BUF [data->READ_BUF_LENGTH]));
#endif

    data->READ_BUF_LENGTH += status;

    /* don't handle a command if busy! */

    /* what the heck, handle a command */
    while (line = grab_line (data->READ_BUF, &data->READ_BUF_LENGTH)) {

        /* need to keep the whole line, for pushback */
        
#ifdef debug_output
    printf ("Processing line: %s\n", line);
#endif

        if (get_cmd_args (line, cmd, args)) {

#ifdef debug_output
    printf ("Processing command: %s <%s>\n", cmd, args);
#endif

            if (strstr (cmd, "ABOR") != NULL) {
                /* ABOR usually comes with telnet control characters
                   that should be stripped out */
                reset_data_connection (data);
                ctl_write_line (data, "226 Closing data connection");
            } else if (strcmp (cmd, "QUIT") == 0) {
                reset_data_connection (data);
                data->DONE = 1;
                disable_read_callback (sock);
                ctl_write_line (data, "221 Bye!");
                /* the ftp_data will be deleted once all its callbacks
                   are done */
            } else if (data->XFER) {
                printf ("not going to handle commands while transferring!\n");
                /* if transferring, don't handle any other commands */
                i = strlen (line) + 2;
                memmove (&(data->READ_BUF [i]), data->READ_BUF, data->READ_BUF_LENGTH);
                data->READ_BUF_LENGTH += i;
                memcpy (data->READ_BUF, line, i - 2);
                data->READ_BUF [i - 2] = '\r';
                data->READ_BUF [i - 1] = '\n';
                free (line);
                return;
            } else if (strcmp (cmd, "USER") == 0) {
                data->AUTHED = 0;
                ctl_write_line (data, "331 Okay, waiting for password.");
            } else if (strcmp (cmd, "PASS") == 0) {
                data->AUTHED = 1;
                ctl_write_msg (data, 230, LOGIN_MESSAGE);
            } else if (strcmp (cmd, "NOOP") == 0) {
                ctl_write_line (data, "200 NOOP command successful");
            /*} else if (!data->AUTHED) {
                // don't allow other commands until logged in
                ctl_write_line (data, "530 Log in with USER and PASS first."); */
            } else if (strcmp (cmd, "SYST") == 0) {
                ctl_write_line (data, "215 MVS Type: L8");
            } else if (strcmp (cmd, "MODE") == 0) {
                if ((args [0] == 'S') || (args [0] == 's')) {
                    ctl_write_line (data, "200 MODE S.");
                } else {
                    ctl_write_line (data, "502 Unimplimented MODE type");
                };
            } else if (strcmp (cmd, "STRU") == 0) {
                if ((args [0] == 'F') || (args [0] == 'f')) {
                    ctl_write_line (data, "200 STRU F.");
                } else {
                    ctl_write_line (data, "504 Unimplimented STRU type");
                };
            } else if (strcmp (cmd, "TYPE") == 0) {
                type = toupper (args [0]);
                if ((type == 'A') || (type == 'I')) {
                    data->TYPE = type;
                    sprintf (tstr, "200 Type set to %c", type);
                    ctl_write_line (data, tstr);
                } else {
                    sprintf (tstr, "504 Type %c not implemented", type);
                    ctl_write_line (data, tstr);
                };
            } else if (strcmp (cmd, "PASV") == 0) {
                reset_data_connection (data);
                /* listen on the same address as the client connected to */
                data->DATA_LISTEN_SOCK = listen_sock (0, sock_ip (sock));
                if (data->DATA_LISTEN_SOCK == -1) {
                    ctl_write_line (data, "500 PASV command unsuccessful");
                } else {
                    data->DATA_SOCK_STATUS = 'L';
                    set_callbacks (data->DATA_LISTEN_SOCK, data, &data_listen_sock_rcb, NULL);

                    comma_list = sockaddr_to_comma_list (data->DATA_LISTEN_SOCK);
                    sprintf (tstr, "227 Entering Passive Mode %s", comma_list);
                    ctl_write_line (data, tstr);
                    free (comma_list);
                };

            } else if (strcmp (cmd, "PORT") == 0) {
                if (!comma_list_to_sockaddr (&addr, args)) {
                    ctl_write_line (data, "500 That's not how to use the PORT command.");
                } else {
                    reset_data_connection (data);
                    data->DATA_CONNECT_ADDR = malloc (sizeof (addr));
                    memcpy (data->DATA_CONNECT_ADDR, &addr, sizeof (addr));
                    data->DATA_SOCK = nb_connect_sock ((void *)&addr, &i);

                    if (data->DATA_SOCK == -1) {
                        sprintf (tstr, "500 PORT command unsuccessful cd: %d", i);
                        ctl_write_line (data, tstr);
                    } else {
                        data->DATA_SOCK_STATUS = 'C';
                        set_callbacks (data->DATA_SOCK, data, NULL, &data_connect_sock_wcb);
                        ctl_write_line (data, "200 PORT command successful");
                    };
                };
            } else if (strcmp (cmd, "LIST") == 0) {
                if (prepare_for_xfer (data)) {
                    tdata = ftp_ls (data);
                    if (tdata) {
                        free (data->DATA_SOCK_BUF);
                        data->DATA_SOCK_BUF = tdata;
                    } else {
                        data->DATA_SOCK_BUF [0] = 0;
                    };

                    data->DATA_SOCK_BUF_LEN = strlen (data->DATA_SOCK_BUF);

                    if (data->DATA_SOCK_BUF_LEN) {
#ifdef debug_output
                        printf ("Directory: %s\n", data->DATA_SOCK_BUF);
#endif
                        ebcdic2ascii (data->DATA_SOCK_BUF, data->DATA_SOCK_BUF_LEN);
                    };

                    data->XFER = 'R'; /* the client is reading the file listing */
                    set_xfer_callback (data);
                };
            } else if (strcmp (cmd, "PWD") == 0) {
                sprintf (tstr, "257 \"%s\" is current directory.", data->CWD);
                ctl_write_line (data,  tstr);
            } else if (strcmp (cmd, "CWD") == 0) {
                if (ftp_chdir (data, args) == 1) {
                    ctl_write_line (data, "250 CWD command successful");
                } else {
                    sprintf (tstr, "550 Can't cd into %s.", args);
                    ctl_write_line (data, tstr);
                };
            } else if (strcmp (cmd, "CDUP") == 0) { // Should not need a dir-refresh on this one
                if (ftp_chdir (data, "..")) {
                    ctl_write_line (data, "250 CDUP command successful");
                } else {
                    ctl_write_line (data, "550 Can't cd into ..");
                };
            } else if (strcmp (cmd, "RETR") == 0) {

                tpath = dir_concat (data->CWD, args);
                strcpy (real_path, "//DSN:");
                strcpy (&(real_path [6]), &(tpath [1])); /* Remove root slash */
                strupr (real_path);
                free (tpath);

                if (real_path [6] != 0) {
                    i = strlen (real_path);
                    real_path [i - 1] = 0; /* Remove trailing slash */
                    s = strstr (&(real_path [6]), "/");
                    if (s == NULL) { /* open root file */

                        j = 0; // Check if the name matches a known user DD
                        while (j < added_ddc) {
                            if (strcmp (added_dds [j], &(real_path [6])) == 0) {
                                real_path [3] = 'D'; // Convert to //DDN:...
                                break;
                            }
                            j++;
                        }
                        if (j == added_ddc) { // Not a user DD, continue checking

                            org = getorg (&(real_path [6]));
                            if (org == 0) { // File wasn't found
                                refresh ();
                                org = getorg (&(real_path [6])); // try again now
                            }

                            if (org != 1) /* First check it's PS */
                                real_path [0] = 0;
                        }
                    } else {         /* open pds file */
                        s [0] = '(';
                        real_path [i - 1] = ')';
                    };

#ifdef debug_output
                    if (real_path [0] != 0) printf ("Opening filename: %s (Read)\n", real_path);
#endif

                    if (data->TYPE == 'A') {
                        strcpy (fmode, "r");
                    } else {
                        strcpy (fmode, "rb");
                    };

                    if (real_path [0] == 0) {
                        
                        sprintf (tstr, "553 %s: Access denied.", args);
                        ctl_write_line (data, tstr);

                    } else if ((fh = fopen (real_path, fmode)) == NULL) {

                        sprintf (tstr, "550 %s: %d", args, errno);
                        ctl_write_line (data, tstr);

                    } else {
                        if (prepare_for_xfer (data)) {
                            data->FILE = fh;
#ifdef debug_output
                            printf ("Opened file: %d read\n", (int)data->FILE);
#endif
                            set_callbacks_f (fh, data, file_rcb, NULL);
                            data->XFER = 'R'; /* the client is reading the file */
                            set_xfer_callback (data);
                        };
                    };
                } else {
                    sprintf (tstr, "553 root: Access denied.");
                    ctl_write_line (data, tstr);
                };
            } else if (strcmp (cmd, "STOR") == 0) {

                tpath = dir_concat (data->CWD, args);
                strcpy (real_path, "//DSN:");
                strcpy (&(real_path [6]), &(tpath [1])); /* Remove root slash */
                strupr (real_path);
                free (tpath);

                if (real_path [6] != 0) {
                    i = strlen (real_path);
                    real_path [i - 1] = 0; /* Remove trailing slash */

                    s = strstr (&(real_path [6]), "/");
                    if (s == NULL) { /* open root file */

                        j = 0; // Check if the name matches a known user DD
                        while (j < added_ddc) {
                            if (strcmp (added_dds [j], &(real_path [6])) == 0) {
                                real_path [3] = 'D'; // Convert to //DDN:...
                                break;
                            }
                            j++;
                        }
                        if (j == added_ddc) { // Not a user DD, continue checking

                            org = getorg (&(real_path [6]));
                            if (org == 0) { // File wasn't found
                                refresh ();
                                org = getorg (&(real_path [6])); // try again now
                            }

                            if (org != 1) /* First check it's PS */
                                real_path [0] = 0;
                        }
                    } else {         /* open pds file */
                        s [0] = '(';
                        real_path [i - 1] = ')';
                    };

#ifdef debug_output
                    if (real_path [0] != 0) printf ("Opening filename: %s (Write)\n", real_path);
#endif

                    if (data->TYPE == 'A') {
                        strcpy (fmode, "w");
                    } else {
                        strcpy (fmode, "wb");
                    };

                    if (real_path [0] == 0) {
                        
                        sprintf (tstr, "553 %s: Access denied.", args);
                        ctl_write_line (data, tstr);

                    } else if ((fh = fopen (real_path, fmode)) == NULL) {
                        
                        sprintf (tstr, "550 %s: Not opened %d", args, errno);
                        ctl_write_line (data, tstr);

                    } else {
                        if (prepare_for_xfer (data)) {
                            data->FILE = fh;
#ifdef debug_output
                            printf ("Opened file: %d write\n", (int)data->FILE);
#endif
                            set_callbacks_f (fh, data, NULL, file_wcb);
                            data->XFER = 'W'; /* the client is writing the file */
                            set_xfer_callback (data);
                        };
                    };
                } else {
                    sprintf (tstr, "553 root: Access denied.");
                    ctl_write_line (data, tstr);
                };
            
            } else if (strcmp (cmd, "TERMINATE") == 0) {
                running = 0;
            } else if (strcmp (cmd, "TERM") == 0) {
                running = 0;
            } else if (strcmp (cmd, "REFR") == 0) {
                refresh ();
                ctl_write_line (data, "200 REFRESH command successful");
            } else if (strcmp (cmd, "REFRESH") == 0) {
                refresh ();
                ctl_write_line (data, "200 REFRESH command successful");
            } else {
                ctl_write_line (data, "500 Huh?");
            };
        } else {
            ctl_write_line (data, "501 Huh?");
        };

        free (line);
    };
};

/*
# writes out buffered data, and removes the callback when DONE is true
*/
static void ftp_sock_wcb (SOCKET sock, data_tag_ptr data) {
    long bytes_written;

#ifdef debug_output
    data->WRITE_BUF [data->WRITE_BUF_LENGTH] = 0;
    printf ("Sending %d bytes: %s\n", data->WRITE_BUF_LENGTH, data->WRITE_BUF);
#endif

    if (data->WRITE_BUF_LENGTH) {
        ebcdic2ascii (data->WRITE_BUF, data->WRITE_BUF_LENGTH);
    };

    /* just write whatever's available to be written */
    bytes_written = write_from_buf (sock, data->WRITE_BUF, data->WRITE_BUF_LENGTH);

    if (bytes_written == 0) {
        printf ("error writing to control connection!\n");
        reset_data_connection (data);
        remove_callbacks (sock);

        if (sock != -1) {
#ifdef debug_output
            printf ("Closing down socket: %d\n", sock);
#endif
            closesocket (sock);
        };

        return;
    };

    data->WRITE_BUF_LENGTH -= bytes_written;
    
    /* if we've written all the data, disable write callbacks */
    if (data->WRITE_BUF_LENGTH == 0) {
        disable_write_callback (sock);
        if (data->DONE) {
            remove_callbacks (sock);

            if (sock != -1) {
#ifdef debug_output
                printf ("Closing down socket: %d\n", sock);
#endif
                closesocket (sock);
            };
        };
    } else { /* Should never happen, but... */
        ascii2ebcdic (data->WRITE_BUF, data->WRITE_BUF_LENGTH);
    };
};

/*
# accept a new connection and register callbacks for it
*/
static void ftp_serv_sock_rcb (SOCKET serv_sock, data_tag_ptr data) {
    SOCKET acc;

    acc = accept_sock (serv_sock);
    
    if (acc == -1) {
        printf ("error accepting connection: %d\n", HercGetLastError ());
        running = 0;
        return;
    };

    set_callbacks (acc, new_ftp_data (acc), &ftp_sock_rcb, &ftp_sock_wcb);
};

void main (int argc, char ** argv) {
    long   serv_ip;
    SOCKET serv_sock;
    long   i;
    long   j;
    data_tag_ptr data;
    cb_data_ptr ohead;

    p_root r;
    int dircount = 0;

    if ((argc >= 2) && (stricmp (argv [1], "fast") == 0)) {
        printf ("Library Optimisation Extensions Enabled\n");
        __libc_arch = 1; /* Enable speed optimisations within the library */
        j = 2;
    } else
        j = 1;

    while ((j < argc) && (added_ddc < added_ddl)) {
        if (strnicmp (argv [j], "//DDN:", 6) == 0) {
            strncpy (added_dds [added_ddc], &(argv [j][6]), 8);
            added_dds [added_ddc][8] = 0;
            strupr (added_dds [added_ddc++]);
        }
        j++;
    }

#ifdef debug_output
        printf ("Init...\n");
#endif
    i = init ();

#ifdef debug_output
        printf ("Directory Read...\n");
#endif
    i += dirsupport ();

#ifdef debug_output
        printf ("VTOC Reads... (%d devices)\n", numdevs);
#endif

    if (i == 0) {
        r = (p_root)malloc (sizeof (t_root));
        if (r == NULL) {
            printf ("Out of memory starting, vtoc stage\n");
            return;
        }
        r->dircount = 0;
        r->next = NULL;
        j = 0;
        while (j < numdevs) {
            i += readvtoc (devices [j], units [j], r);
            j++;
        };
        root = r;
    };

    if (i != 0) {
        printf ("Couldn't initialise\n");
        return;
    };

//#ifdef debug_output
    r = root;
    while (r) {
        dircount += r->dircount;
        r = r->next;
    }
    printf ("Starting... (%d datasets)\n", dircount);
//#endif

    if (strcmp (SERVER_IP, "any") != 0) {
        inet_aton (SERVER_IP, (void *)&serv_ip);
    } else {
        serv_ip = htonl (INADDR_ANY);
    };

    serv_sock = listen_sock (SERVER_PORT, serv_ip);
    if (serv_sock < 0) {
        printf ("Can't Listen error %d, is FTP already running on Port %d?\n", errno, SERVER_PORT);
    } else {
#ifdef debug_output
        printf ("Listening on Socket: %d, Port: %d\n", serv_sock, SERVER_PORT);
#endif
        set_callbacks (serv_sock, NULL, &ftp_serv_sock_rcb, NULL);

        run_callbacks ();

        while (head) {
            ohead = head;
            data = head->FUNC_DATA;

            if (data == NULL) {
                if (ohead->FH != -1) {
#ifdef debug_output
                    printf ("Closing down socket: %d\n", ohead->FH);
#endif
                    closesocket (ohead->FH);
                };
                head = head->next;
                free (ohead);
            };

            while (data) {
                remove_callbacks_f (data->FILE);

                if (ohead != head) break; /* Was completely removed! */

                if (data->FILE) {
#ifdef debug_output
                    printf ("Closing down file: %d\n", (int)data->FILE);
#endif
                    fclose (data->FILE);
                };
                data->FILE = NULL;

                remove_callbacks (data->DATA_SOCK);

                if (ohead != head) break; /* Was completely removed! */
            
                if (data->DATA_SOCK != -1) {
#ifdef debug_output
                    printf ("Closing down socket: %d\n", data->DATA_SOCK);
#endif
                    closesocket (data->DATA_SOCK);
                };
                data->DATA_SOCK = -1;
    
                remove_callbacks (data->DATA_LISTEN_SOCK);
            
                if (ohead != head) break; /* Was completely removed! */

                if (data->DATA_LISTEN_SOCK != -1) {
#ifdef debug_output
                    printf ("Closing down socket: %d\n", data->DATA_LISTEN_SOCK);
#endif
                    closesocket (data->DATA_LISTEN_SOCK);
                }
                data->DATA_LISTEN_SOCK = -1;

                if (data->SOCK != -1) {
#ifdef debug_output
                    printf ("Closing down socket: %d\n", data->SOCK);
#endif
                    closesocket (data->SOCK);
                }
                remove_callbacks (data->SOCK);

                data->SOCK = -1; // This isn't really needed!
                if (ohead == head)
                    head = head->next; // Something bad happened, ignore this entry.

                break;
            };
        };
    };
//#ifdef debug_output
        printf ("Ending...\n", dircount);
//#endif
};
