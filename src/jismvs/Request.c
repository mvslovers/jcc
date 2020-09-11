/*
 * JIS MVS3.8j WWW Application Server.  (C)2004 Jason Paul Winter, All Rights Reserved.
 *
 * jasonwinter@hotmail.com, use "Subject" wisely - spam gets deleted without being read.
 *
 */

#include <stdlib.h>
#include <sockets.h>
#include <string.h>

#define Request 1
#include "asp.h"
#include "web.h"

extern unsigned char __upper1 [];
extern WebSite_ptr wp;

// Request object.
//
// Reads from the socket...  Placing information into a series
// of collections...  Also extracts the path & file requested.

// Properties

//static COLLECT * ClientCertificate; // Not implemented.

static COLLECT * Cookies;
static COLLECT * Form;
static COLLECT * QueryString;
static COLLECT * ServerVariables;

static long TotalBytes = 0; // How many bytes read from soc?

static SOCKET soc = -1;

static unsigned long client_ip;  // Set by main
static unsigned long host_ip;    // Obtained here *after* some IO!

static long post = 1; // or get = 0

static long start_time; // log entry needs this.

// this webserver only serves from PDSs...
static char Path [45]; // pds
static char File [9];  // member
static char Ext [4];   // extension

static char Msg [81];   // An error indicator
static char * Cookie = NULL; // Pointer into the ASPSESSIONID cookie.

static long http_version = 1; // default to 1.1 (not 1.0)

// Methods

static VARIANT * (*Default) (); // c_Search - (bitmask=-1)
static void (*Start) ();
static void (*Form_Start) ();
static void (*QueryString_Start) ();

// Globals

static int unpacked1 = 0; // Flag 'not processed'
static int unpacked2 = 0; // Flag 'not processed'

static char * qs = NULL;
static char * rf = NULL;

// Code

// A COLLECT * Search
static void * c_Search (char * key, long bitmask) {
    void * v;

    // bitmask values:
    // ---------------
    // QueryString = 0000 0000 0000 0000 0000 0000 0000 0001 = 1
    // Form        = 0000 0000 0000 0000 0000 0000 0000 0010 = 2
    // Cookies     = 0000 0000 0000 0000 0000 0000 0000 0100 = 4
    // ServerVars. = 0000 0000 0000 0000 0000 0000 0000 1000 = 8

    if (bitmask & 0x00000001) {
        v = QueryString->Default (key);
        if (v) return (v);
    };
    if (bitmask & 0x00000002) {
        v = Form->Default (key);
        if (v) return (v);
    };
    if (bitmask & 0x00000004) {
        v = Cookies->Default (key);
        if (v) return (v);
    };
    if (bitmask & 0x00000008) {
        v = ServerVariables->Default (key);
        if (v) return (v);
    };

    return (NULL); // Was not found anywhere.
};

static void c_Start () {
    char c1;
    char c2;
    long i;
    long j;
    long j2;
    long k;
    long l;
    long m;
    long n;
    long s;
    long s1;
    long s2;
    char * t;
    char * t2;
    char * t3;
    char * t4;
    char * t5;
    char * t6;
    char buf [8193]; // Any larger-a-header, and they must be hackers!
    char buf2 [12];
    long content_length = 0; // Assume none (Form POST Data Length.)
    VARIANT * o;
    list_ptr lp;
    char * crlfcrlf;
    long   content_added;
    char   URL [2049]; // Allow a largish one
    int    sz_sa = sizeof (struct sockaddr);
    struct sockaddr sa;
    struct sockaddr_in * sap = (struct sockaddr_in *)&sa;

    Ext [0] = 0;
    Msg [0] = 0;
    URL [0] = 0;

    // Read from soc now...
    // Accept POST or GET... but get a large buffer-full first...
    s = 0;
    s1 = 0;
    s2 = 0;
    buf [0] = 0;
    while ((crlfcrlf = strstr (&(buf [s2]), "\x0D\x0A\x0D\x0A")) == NULL) { // ASCII CRLFs

        if (((l = recv (soc, &(buf [s]), 8192 - s, 0)) == SOCKET_ERROR) ||
            (l == 0) ||
            (s == 8192)) {

            sprintf (Msg, "HTTP Request Header was malformed and didn't end");
            strcpy (Path, "/");
            strcpy (File, "error");
            strcpy (Ext, "asp");
            return;
        };
        TotalBytes += l;
        s += l;
        buf [s] = 0;

        if (s1 > 4)
            s2 = s1 - 4; // else leave s2 as 0
        if (s > 4)
            s1 = s - 4;  // else leave s1 as 0
    };

    // ebcdic
    atoe (buf, 4);

    if (memcmp (buf, "GET ", 4) == 0)
        post = 0;
    else
    if (memcmp (buf, "POST", 4) == 0)
        post = 1;
    else {
        buf [5] = 0;
        sprintf (Msg, "HTTP Methods 'GET' and 'POST' are supported: '%s' isn't", buf);
        strcpy (Path, "/");
        strcpy (File, "error");
        strcpy (Ext, "asp");
        return;
    };
    
    l = 4;
    while (buf [l] == 32) l++; // ASCII SPACE
    j = l;  // j is the start of the URL

    t3 = NULL;
    t2 = memchr (&(buf [l]), 63, s - l); // ASCII '?'
    t = memchr (&(buf [l]), 32, s - l); // Is usual to have a Space delimit a URL requested
    
    if (t == NULL) {
        sprintf (Msg, "HTTP Methods 'GET' and 'POST' require \" HTTP/1.v\"");
        strcpy (Path, "/");
        strcpy (File, "error");
        strcpy (Ext, "asp");
        return;
    };

    if ((t2 != NULL) && (t != NULL) && ((long)t2 < (long)t)) {
        t3 = t;
        t = t2; // Use whichever came first (t2 in this case)
    };

    l = (long)t - (long)buf; // [l] now points to URL ? or end
    if (s - l)                    // Check for end?
        atoe (&(buf [l]), s - l); // * No: Translate QueryString to finish...

    i = l - j; // Length of URL but not QueryString

    if (t3) {
        // Save a copy of the QueryString in RAW form...
        qs = (char *)malloc ((long)t3 - (long)t);
        memcpy (qs, &(t [1]), ((long)t3 - (long)t) - 1);
        qs [((long)t3 - (long)t) - 1] = 0;

        l = (long)t3 - (long)&(buf [0]); // fix: l - to be the URL and QueryString
    };

    if (i > 2048) i = 2048; // Is it too long for a URL & no QueryString?
    memcpy (URL, &(buf [j]), i);

    // convert '%hh' hex-sequences...
    i = URLDecode (URL, i);
    atoe (URL, i);
    URL [i] = 0;
    if (URL [0] != '/') {
        sprintf (Msg, "The URL requested is invalid for this system");
        strcpy (Path, "/");
        strcpy (File, "error");
        strcpy (Ext, "asp");
        return;
    };

/* (example input: for URL:"HTTP://127.0.0.1/" -> "/" when no proxy is used.)
GET / HTTP/1.1
Host: 127.0.0.1
Connection: Keep-Alive
*/

    k = 0; // . hasn't been found.
    i--;   // End of URL found in i.
    while ((URL [i] != '.') && (URL [i] != '/')) i--;
    if (URL [i] == '.') {
        URL [i] = 0;
        k = i;
        strncpy (Ext, &(URL [i + 1]), 3);
        Ext [3] = 0;
    };
    while (URL [i] != '/') i--;
    c1 = URL [++i];
    if (c1 == 0) { // Was the end (no file or just an extension.)
        if (k) URL [k] = '.';
        sprintf (Msg, "No 'default' requests allowed yet ***");
        strcpy (Path, "/");
        strcpy (File, "error"); // Code some *default* lookup functions
        strcpy (Ext, "asp");
        return;
    } else {       // Put things back...
        URL [i] = 0;
        strncpy (Path, URL, 44);
        Path [44] = 0;
        URL [i] = c1;
        strncpy (File, &(URL [i]), 8);
    };
    if (k) URL [k] = '.';

    // Should end in " HTTP/1.0\r\n" or similar...
    // all buffers are EBCDIC now except the *Undecoded* QueryString in buf.

    if ((buf [l] != '\r') && (strnicmp (&(buf [l]), " HTTP", 5) != 0)) {
        buf [l + 5] = 0;
        sprintf (Msg, "Non-standard URL request type:%s", &(buf [l]));
        strcpy (Path, "/");
        strcpy (File, "error");
        strcpy (Ext, "asp");
        return;
    };
    if (buf [l + 8] == '0') http_version = 0;
    while ((buf [l] != '\r') && (buf [l] != '\n')) l++;

    j = ServerVariables->No_Errors;
    j2 = Cookies->No_Errors;
    ServerVariables->No_Errors = 1;
    Cookies->No_Errors = 1;

    // Add URL

    o = String (URL);
    ServerVariables->Add (o, "URL", NULL, NULL);
    _del_object (o); // Free Variant & String iff* server-variable *wasn't* updated!

    // Add ALL_RAW

    m = l;
    crlfcrlf [0] = 0; // points to end

    o = String (&(buf [m + 2]));
    ServerVariables->Add (o, "ALL_RAW", NULL, NULL);
    _del_object (o); // Free Variant & String iff* server-variable *wasn't* updated!

    crlfcrlf [0] = '\r';

    // Continue to read lines (ending in '\r\n'... until '\r\n\r\n')
    content_added = 0;
    while (((long *)&(buf [l])) [0] != 0x0D150D15) {

        l += 2;  // Skip \r\n which must begin here...
        while (buf [l] == ' ') l++;

        // Content-Length= is a special consideration
        if ((strnicmp (&(buf [l]), "Content-Length", 14) == 0) &&
            ((buf [l + 14] == ' ') ||
             (buf [l + 14] == ':') ||
             (buf [l + 14] == '='))) {

            i = 0;
            while (++i <= 14) {
                buf [l] = __upper1 [buf [l]];
                l++;
            };
            while (buf [l] == ' ') l++;
            while (buf [l] == ':') l++;
            while (buf [l] == '=') l++;
            while (buf [l] == ' ') l++;
            // Actually calculate the number into content_length...
            content_length = atol (&(buf [l]));

            itoa (content_length, buf2, 10);
            o = String (buf2);
            ServerVariables->Add (o, "CONTENT_LENGTH", NULL, NULL);
            _del_object (o); // Free Variant & String iff* server-variable *wasn't* updated!

            content_added = 1;

            t = memchr (&(buf [l]), '\r', s - l);
            l += ((long)t - (long)&(buf [l]));

        } else { // Just create a collection item

            i = l;
            while ((buf [l] != ' ') && (buf [l] != '=') && (buf [l] != ':')) {
                if (buf [l] == '-')
                    buf [l] = '_';
                else
                    buf [l] = __upper1 [buf [l]];
                l++;
            };
            k = l;

            while (buf [l] == ' ') l++;
            while (buf [l] == ':') l++;
            while (buf [l] == '=') l++;
            while (buf [l] == ' ') l++;
            n = k;
            c1 = buf [n];
            buf [n] = 0;
            k = l;

            t = memchr (&(buf [l]), '\r', s - l);
            l += ((long)t - (long)&(buf [l]));
            buf [l] = 0;

//Production Server Cookie:
//TESTCOOKIE=true; HIDEWELCOMEWINDOW=1; CFID=102; CFTOKEN=24989790; ASPSESSIONIDQQQGQWDC=GPOPAFMBKCAEACNBEJEKKKDC

            if (strcmp (&(buf [i]), "COOKIE") == 0) {

                // Break-Up the parts of the cookie...
                t = &(buf [k]);
                while ((t) && (*t)) {

                    if ((Cookie == NULL) &&
                        (memcmp (t, "ASPSESSIONID", 12) == 0)) {

                        Cookie = (char *)malloc (32);
                        memcpy (Cookie, &(t [12]), 25);
                        Cookie [25] = 0;
                    };

                    // Insert the cookie element into the Cookies Collection...
                    //
                    // Dictionary:
                    // Set-Cookie:MYCOOKIE=TYPE1=sugar&TYPE2=ginger+snap

                    t6 = strchr (t, ';');
                    if (t6) t6 [0] = 0;

                    t2 = strchr (t, '=');
                    if (t2) {
                        t2 [0] = 0;

                        t3 = strchr (&(t2 [1]), '&');
                        t5 = &(t2 [1]);
                        if (t3) { // Dictionary...
                            do {
                                t3 = strchr (t5, '&');
                                if (t3) {
                                    t3 [0] = 0;
                                };

                                t4 = strchr (t5, '=');
                                if (t4 == NULL) break;

                                t4 [0] = 0;
                                o = String (&(t4 [1]));

                                ((STRING *)(o->Value.ov))->Length = URLDecodeE (((STRING *)(o->Value.ov))->Value, ((STRING *)(o->Value.ov))->Length + 1, __ascii_to_ebcdic) - 1;

                                Cookies->AddKeyed (o, t, t5);
                                _del_object (o); // Free Variant & String iff* Cookies *wasn't* updated!
                                t4 [0] = '=';

                                if (t3) {
                                    t3 [0] = '&';
                                    t5 = &(t3 [1]);
                                };

                            } while (t3);

                        } else {

                            o = String (&(t2 [1]));

                            ((STRING *)(o->Value.ov))->Length = URLDecodeE (((STRING *)(o->Value.ov))->Value, ((STRING *)(o->Value.ov))->Length + 1, __ascii_to_ebcdic) - 1;

                            lp = Cookies->Add (o, t, NULL, NULL); // lp is for a test only!
                            _del_object (o); // Free Variant & String iff* Cookies *wasn't* updated!
                        };

                        t2 [0] = '=';
                    };
                    t = t6;
                    if (t) {
                        t [0] = '=';
                        t++;
                        while (*t == ' ') t++;
                    };
                };
            };

            o = String (&(buf [k]));

            ((STRING *)(o->Value.ov))->Length = URLDecodeE (((STRING *)(o->Value.ov))->Value, ((STRING *)(o->Value.ov))->Length + 1, __ascii_to_ebcdic) - 1;

            lp = ServerVariables->Add (o, &(buf [i]), NULL, NULL); // lp is for a test only!
            _del_object (o); // Free Variant & String iff* server-variable *wasn't* updated!

            buf [n] = c1;
            buf [l] = '\r';
        };
    };

    // content_added!
    if (content_added == 0) {
        o = String ("0");
        ServerVariables->Add (o, "CONTENT_LENGTH", NULL, NULL);
        _del_object (o); // Free Variant & String iff* server-variable *wasn't* updated!
    };

    // Add ALL_HTTP (Now that upper-casing is performed)

    crlfcrlf [0] = 0; // points to end

    o = String (&(buf [m + 2]));
    ServerVariables->Add (o, "ALL_HTTP", NULL, NULL);
    _del_object (o); // Free Variant & String iff* server-variable *wasn't* updated!

    crlfcrlf [0] = '\r';

    // Add LOCAL_ADDR      = 127.0.0.1
    getsockname (soc, &sa, &sz_sa);
    host_ip = sap->sin_addr.s_addr; // is the client-browser or proxy.
    sprintf (buf, "%d.%d.%d.%d",
        (host_ip & 0xFF000000) >> 24,
        (host_ip & 0x00FF0000) >> 16,
        (host_ip & 0x0000FF00) >> 8,
         host_ip & 0x000000FF);
    o = String (buf);
    ServerVariables->Add (o, "LOCAL_ADDR", NULL, NULL);
    _del_object (o); // Free Variant & String iff* server-variable *wasn't* updated!

    // Add REMOTE_ADDR     = 127.0.0.1
    sprintf (buf, "%d.%d.%d.%d",
        (client_ip & 0xFF000000) >> 24,
        (client_ip & 0x00FF0000) >> 16,
        (client_ip & 0x0000FF00) >> 8,
         client_ip & 0x000000FF);
    o = String (buf);
    ServerVariables->Add (o, "REMOTE_ADDR", NULL, NULL);
    _del_object (o); // Free Variant & String iff* server-variable *wasn't* updated!

    // Add REMOTE_HOST     = 127.0.0.1
    o = String (buf);
    ServerVariables->Add (o, "REMOTE_HOST", NULL, NULL);
    _del_object (o); // Free Variant & String iff* server-variable *wasn't* updated!

    // Add SERVER_NAME     = 127.0.0.1
    o = String (wp->name);
    ServerVariables->Add (o, "SERVER_NAME", NULL, NULL);
    _del_object (o); // Free Variant & String iff* server-variable *wasn't* updated!

    // Add SERVER_PORT     = 80
    sprintf (buf, "%d", wp->open_port);
    o = String (buf);
    ServerVariables->Add (o, "SERVER_PORT", NULL, NULL);
    _del_object (o); // Free Variant & String iff* server-variable *wasn't* updated!

    // Add PATH_INFO       = /testasp.asp
    o = String (URL);
    ServerVariables->Add (o, "PATH_INFO", NULL, NULL);
    _del_object (o); // Free Variant & String iff* server-variable *wasn't* updated!

    // Add SCRIPT_NAME     = /testasp.asp
    o = String (URL);
    ServerVariables->Add (o, "SCRIPT_NAME", NULL, NULL);
    _del_object (o); // Free Variant & String iff* server-variable *wasn't* updated!

    // Add PATH_TRANSLATED = c:\inetpub\wwwroot\testasp.asp
    sprintf (buf, "//DDN:ASPSRCS(%s)", File);
    strupr (buf);
    o = String (buf);
    ServerVariables->Add (o, "PATH_TRANSLATED", NULL, NULL);
    _del_object (o); // Free Variant & String iff* server-variable *wasn't* updated!
    
    // Add QUERY_STRING    = ddd=fgh&eee=mgh
    if (qs) {

        o = String (qs);
        ServerVariables->Add (o, "QUERY_STRING", NULL, NULL);
        _del_object (o); // Free Variant & String iff* server-variable *wasn't* updated!
    };

    // Add REQUEST_METHOD  = GET|POST
    if (post)
        o = String ("POST");
    else
        o = String ("GET");
    ServerVariables->Add (o, "REQUEST_METHOD", NULL, NULL);
    _del_object (o); // Free Variant & String iff* server-variable *wasn't* updated!

    // Add SERVER_SOFTWARE = JCC-ASP1.0
    o = String ("JCC-ASP1.0");
    ServerVariables->Add (o, "SERVER_SOFTWARE", NULL, NULL);
    _del_object (o); // Free Variant & String iff* server-variable *wasn't* updated!

    // Add SERVER_PROTOCOL = HTTP/1.1
    sprintf (buf, "HTTP/1.%d", http_version);
    o = String (buf);
    ServerVariables->Add (o, "SERVER_PROTOCOL", NULL, NULL);
    _del_object (o); // Free Variant & String iff* server-variable *wasn't* updated!

    ServerVariables->No_Errors = j;
    Cookies->No_Errors = j2;

    // If POSTing, then get that data too now... Might use Content-Length?
    if (post) {
        
        if (content_length == 0) {
            sprintf (Msg, "HTTP POST Data was missing a Content-Length");
            strcpy (Path, "/");
            strcpy (File, "error");
            strcpy (Ext, "asp");
            return;
        };
        if (content_length >= 1000000) { // Threads*Memory causes any problems?
            sprintf (Msg, "HTTP POST Data was too large (1MB limit.)");
            strcpy (Path, "/");
            strcpy (File, "error");
            strcpy (Ext, "asp");
            // Don't Return yet...
            while (s < content_length) {
                l = recv (soc, &(buf [s]), 8192 - s, 0);
                if ((l == 0) || (l == SOCKET_ERROR)) return;
                TotalBytes += l;
                s += l;
            };
            if ((content_length + 2) - s == 2) {
                l = recv (soc, buf, 2, 0); // CRLF, if needed.
                TotalBytes += l;
            } else
            if ((content_length + 2) - s == 1) {
                l = recv (soc, buf, 1, 0); // just LF, if needed.
                TotalBytes += l;
            };
            return;
        };

        l += 4;
        s -= l;

        if (content_length <= 0x01000000) // 16MB Limit?!
            rf = (char *)malloc (content_length + 1);
        else
            rf = NULL;

        if (rf == NULL) {
            sprintf (Msg, "POST Data would need to much Heap memory");
            strcpy (Path, "/");
            strcpy (File, "error");
            strcpy (Ext, "asp");
            // Don't Return yet...
            while (s < content_length) {
                l = recv (soc, &(buf [s]), 8192 - s, 0);
                if ((l == 0) || (l == SOCKET_ERROR)) return;
                TotalBytes += l;
                s += l;
            };
            if ((content_length + 2) - s == 2) {
                l = recv (soc, buf, 2, 0); // CRLF, if needed.
                TotalBytes += l;
            } else
            if ((content_length + 2) - s == 1) {
                l = recv (soc, buf, 1, 0); // just LF, if needed.
                TotalBytes += l;
            };
            return;
        };

        memcpy (rf, &(buf [l]), s);

        while (s < content_length) { // ASCII CRLFs
            l = recv (soc, &(rf [s]), content_length - s, 0);
            if ((l == SOCKET_ERROR) ||
                (l == 0)) {
                free (rf);
                rf = NULL;
                sprintf (Msg, "HTTP POST Data was malformed");
                strcpy (Path, "/");
                strcpy (File, "error");
                strcpy (Ext, "asp");
                return;
            };
            TotalBytes += l;
            s += l;
        };
        rf [content_length] = 0;

        if ((content_length) && (post) && (http_version == 0)) {

            if ((content_length + 2) - s == 2) {
                l = recv (soc, buf, 2, 0); // CRLF, if needed.
                TotalBytes += l;
            } else
            if ((content_length + 2) - s == 1) {
                l = recv (soc, buf, 1, 0); // just LF, if needed.
                TotalBytes += l;
            };
        };

        // Add "Form" Variables // Not Yet!  Allow USER to choose if we break it up!
        // - need a flag to say 'nothing done yet.'

        // Add "QueryString" Variables // Ditto above "not yet"
        // - these may not be quick to process, and another program may only want
        // -- the entire string anyway - a waste of cycles...
    };
};

static void c_QueryString_Start () {
    long   i;
    long   j;
    long   k;
    long   l;
    long   n;
    VARIANT * o;
    char * t;

    // Use qs (it will be in Encoded/ASCII EBCDIC form)

    if ((unpacked2) || (qs == NULL) || (qs [0] == 0)) return;
    unpacked2 = 1;

    j = QueryString->No_Errors;
    QueryString->No_Errors = 1;

    l = 0;
    while (qs [l] != 0) {

        while (qs [l] == ' ') l++;

        i = l;
        while ((qs [l] != '=') && (qs [l] != '&')) l++;
        if (qs [l] == '&') {
            l++;
            continue;
        };
        n = l;

        while (qs [l] == '=') l++;
//        while (qs [l] == ' ') l++; // ** Don't need to skip spaces
        qs [n] = 0;
        k = l;

        t = strchr (&(qs [l]), '&');
        if (t)
            l = ((long)t - (long)qs);
        else
            l += strlen (&(qs [l]));

        qs [l] = 0;

        o = String (&(qs [k]));

        ((STRING *)(o->Value.ov))->Length = URLDecodeE (((STRING *)(o->Value.ov))->Value, ((STRING *)(o->Value.ov))->Length + 1, __ascii_to_ebcdic) - 1;

        QueryString->Add (o, &(qs [i]), NULL, NULL);
        _del_object (o); // Free Variant & String iff* server-variable *wasn't* updated!

        qs [n] = '=';
        if (t) {
            qs [l] = '&';
            l++;
        };
    };

    QueryString->No_Errors = j;
};

static void c_Form_Start () {
    long   i;
    long   j;
    long   k;
    long   l;
    long   n;
    VARIANT * o;
    char * t;

    // Use rf (it will be in Encoded/ASCII EBCDIC form)

    if ((unpacked1) || (rf == NULL) || (rf [0] == 0)) return;
    unpacked1 = 1;

    j = Form->No_Errors;
    Form->No_Errors = 1;

    l = 0;
    while (rf [l] != 0) {

        while (rf [l] == ' ') l++;

        i = l;
        while ((rf [l] != '=') && (rf [l] != '&')) l++;
        if (rf [l] == '&') {
            l++;
            continue;
        };
        n = l;

        while (rf [l] == '=') l++;
//        while (rf [l] == ' ') l++; // ** Don't need to skip spaces
        rf [n] = 0;
        k = l;

        t = strchr (&(rf [l]), '&');
        if (t)
            l = ((long)t - (long)rf);
        else
            l += strlen (&(rf [l]));

        rf [l] = 0;

        o = String (&(rf [k]));

        ((STRING *)(o->Value.ov))->Length = URLDecodeE (((STRING *)(o->Value.ov))->Value, ((STRING *)(o->Value.ov))->Length + 1, __ascii_to_ebcdic) - 1;

        Form->Add (o, &(rf [i]), NULL, NULL);
        _del_object (o); // Free Variant & String iff* server-variable *wasn't* updated!

        rf [n] = '=';
        if (t) {
            rf [l] = '&';
            l++;
        };
    };

    Form->No_Errors = j;
};

int Request_init (void * me) {

    Default = _make_f_vector (&c_Wrap, &c_Search, me);
    Start = _make_f_vector (&c_Wrap, &c_Start, me);
    Form_Start = _make_f_vector (&c_Wrap, &c_Form_Start, me);
    QueryString_Start = _make_f_vector (&c_Wrap, &c_QueryString_Start, me);

    Cookies = _new_object (Collection_init);
    Form = _new_object (Collection_init);
    QueryString = _new_object (Collection_init);
    ServerVariables = _new_object (Collection_init);

#ifdef DEBUG
    printf ("Request Created.\n");
#endif

    return (1);
};

int Request_exit () {

    _del_f_vector (Default);
    _del_f_vector (Start);
    _del_f_vector (Form_Start);
    _del_f_vector (QueryString_Start);

    _del_object (Cookies);
    _del_object (Form);
    _del_object (QueryString);
    _del_object (ServerVariables);

    if (rf)
        free (rf);

    if (Cookie)
        free (Cookie);

    if (qs)
        free (qs);

#ifdef DEBUG
    printf ("Request Deleted.\n");
#endif

    return (1);
};
