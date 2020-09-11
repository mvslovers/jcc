/*
 * JIS MVS3.8j WWW Application Server.  (C)2004 Jason Paul Winter, All Rights Reserved.
 *
 * jasonwinter@hotmail.com, use "Subject" wisely - spam gets deleted without being read.
 *
 */

#include <stdarg.h>
#include <stdio.h>
#include <sockets.h>

#define Response 1
#include "asp.h"

// Response object.

//***
//*** Does the entire output need to be cached?
//*** - to allow a unique header to be created?
//*** - or to allow a redirect to override data?
//***

#define buf_siz 4096

// Properies

static int send_code = 200; /* More than 4KB means OK! */
static int send_cookie = 0;
static SOCKET soc = -1;
static SESSION * Session = NULL; // Only set if send_cookie = 1

static int TotalBytes = 0;

static long http_version = 1; // Default to HTTP/1.1 not HTTP/1.0

// Methods

static void (*Flush) ();
static void (*Write) ();
static void (*Redirect) ();

// Globals

static long hg = 0;        // Header Gone?
static long bp = 0;        // How much is used?
static long rf = 0;        // Redirected flag.

static char * buf;

static long default_buf = 131072; // Used for buffering (128K)
// This value controls just how large'o-chunk the largest user-script can write
// in *one-single* call to 'Write'...  (It's actually 'default_buf - buf_size'.)

// Code

static void c_header (int code, char * aux) {
    long   i;
    char * a;
    char   sb [8193];
    char   standard_hdr [2049];
/*
    100 Continue
    101 Switching Protocols

    200 OK
    201 Created
    202 Accepted
    203 Non-Authoritative Information
    204 No Content
    205 Reset Content
    206 Partial Content

    300 Multiple Choices
    301 Moved Permanently
    302 Found
    303 See Other
    304 Not Modified
    305 Use Proxy
    306 (Unused)
    307 Temporary Redirect

    400 Bad Request
    401 Unauthorized
    402 Payment Required
    403 Forbidden
    404 Not Found
    405 Method Not Allowed
    406 Not Acceptable
    407 Proxy Authentication Required
    408 Request Timeout
    409 Conflict
    410 Gone
    411 Length Required
    412 Precondition Failed
    413 Request Entity Too Large
    414 Request-URI Too Long
    415 Unsupported Media Type
    416 Requested Range Not Satisfiable
    417 Expectation Failed

    500 Internal Server Error
    501 Not Implemented
    502 Bad Gateway
    503 Service Unavailable
    504 Gateway Timeout
    505 HTTP Version Not Supported
*/

    strcpy (standard_hdr, "\r\nConnection: close");

    switch (code) {
    case 200:
        
        if (send_cookie) {

            // Really generate a unique cookie:
            a = New_Cookie (Session);
            Session = NULL; // Don't delete this Session - it's stored. (non-NULL ones get deleted.)

            // Set the Browser-Return-String...
            sprintf (sb, "HTTP/1.%d 200 OK\r\nSet-Cookie: ASPSESSIONID%s%s\r\n\r\n", http_version, a, standard_hdr);

        } else
            sprintf (sb, "HTTP/1.%d 200 OK%s\r\n\r\n", http_version, standard_hdr);
        break;
    case 301:
        sprintf (sb, "HTTP/1.%d 301 Moved Permanently%s%s\r\n\r\n", http_version, aux, standard_hdr);
        break;
    case 302:
        sprintf (sb, "HTTP/1.%d 302 Found%s%s\r\n\r\n", http_version, aux, standard_hdr);
        break;
    case 400:
        sprintf (sb, "HTTP/1.%d 400 Bad Request%s\r\n\r\n", http_version, standard_hdr);
        break;
    case 401:
        sprintf (sb, "HTTP/1.%d 401 Unauthorised%s\r\n\r\n", http_version, standard_hdr);
        break;
    case 404:
        sprintf (sb, "HTTP/1.%d 404 Not Found%s\r\n\r\n", http_version, standard_hdr);
        break;
    case 500:
        sprintf (sb, "HTTP/1.%d 500 Internal Server Error%s\r\n\r\n", http_version, standard_hdr);
        break;
    case 503:
        sprintf (sb, "HTTP/1.%d 503 Service Unavailable%s\r\n\r\n", http_version, standard_hdr);
        break;
    default:
        sprintf (sb, "HTTP/1.%d 501 Not Implemented%s\r\n\r\n", http_version, standard_hdr);
        break;
    };

    i = strlen (sb);
    etoa (sb, i);
    send (soc, sb, i, 0);

    TotalBytes += i;
};

static void c_Write (char * format, ...) {
    va_list arg;
    long    i;

    va_start (arg, format);
    bp += vsprintf (&(buf [bp]), format, arg);
    va_end (arg);

    if (bp >= buf_siz) { // Flush?

        if (hg == 0) {
            c_header (send_code, "");
            // Construct a Content-Length???
            hg = 1;
        };

        i = 0;
        while (bp >= buf_siz) { // Quick-Flush

            etoa (&(buf [i]), buf_siz);
            send (soc, &(buf [i]), buf_siz, 0);

            bp -= buf_siz;
            i += buf_siz;

            TotalBytes += buf_siz;
        };

        if (bp)
            memcpy (buf, &(buf [i]), bp);
    };
};

static void c_Redirect (char * url) {
    long i;
    long l = strlen (url);
    long l2;
    char sb [8193];

    if (hg == 0) {

        sprintf (sb, "\r\nLocation: %s", url);
        c_header (302, sb);
        
        i = sprintf (sb, "<html>\n<head><title>HTTP 302 Object Moved</title></head>\n<body><a HREF=\"%s", url);

        l2 = URLEncodeE (&(sb [72]), l, __ebcdic_to_ascii);
        i += (l2 - l);

        i += sprintf (&(sb [i]), "\">here</a>\n</body>\n</html>\n");
        
        etoa (sb, i);
        send (soc, sb, i, 0);

        TotalBytes += i;

        hg = 1;
        rf = 1;
    };
};

static void c_Flush () {
    int i;

    if (hg == 0) {
        c_header (send_code, "");
        // Content-Length= needs to be added? - Too hard.
        // Last-Modified: Jan-Dec 01-31 2004 12:00:00

        hg = 1;
    };

    if (bp >= buf_siz) { // Flush partial first
        etoa (buf, bp);
        i = 0;
        while (bp >= buf_siz) {
            send (soc, &(buf [i]), buf_siz, 0);
            bp -= buf_siz;
            i += buf_siz;
            TotalBytes += buf_siz;
        };
        if (bp) {
            send (soc, &(buf [i]), bp, 0);
            TotalBytes += bp;
        };
    } else if (bp) {
        etoa (buf, bp);
        send (soc, buf, bp, 0);
        TotalBytes += bp;
    };
    bp = 0;
};

int Response_init (void * me) {

    buf = (char *)malloc (default_buf);
    if (buf == NULL) return (0);

    Flush = _make_f_vector (&c_Wrap, &c_Flush, me);
    Write = _make_f_vector (&c_Wrap, &c_Write, me);
    Redirect = _make_f_vector (&c_Wrap, &c_Redirect, me);

#ifdef DEBUG
    printf ("Response Created.\n");
#endif

    return (1);
};

int Response_exit () {

    if ((bp) && (rf == 0)) { // Flush closing buffer...

        c_Flush ();
    };
    closesocket (soc);

    _del_f_vector (Flush);
    _del_f_vector (Write);
    _del_f_vector (Redirect);

    if (Session)
        _del_object (Session); // Didn't use the default session...

    free (buf);

#ifdef DEBUG
    printf ("Response Deleted.\n");
#endif

    return (1);
};
