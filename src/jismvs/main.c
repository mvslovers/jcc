/*
 * JIS MVS3.8j WWW Application Server.  (C)2004 Jason Paul Winter, All Rights Reserved.
 *
 * jasonwinter@hotmail.com, use "Subject" wisely - spam gets deleted without being read.
 *
 */

//Temp files MVS style or known DD style?
//
#define MVS 1

// If MVS is not defined, the following are used:
//
//DDN:ASPTEMP1 - The Object Deck
//DDN:ASPTEMP2 - The PreCompiler Output
//DDN:ASPTEMP3 - The Messages

// If admin.asp is built-in, comment out the next line:
#define admin_dyn 1

#ifndef admin_dyn
extern int (*Admin_init) ();
extern int (*Admin_exit) ();
extern int (*Admin_run) ();
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <process.h>
#include <sockets.h>
#include <jccload.h>

#include "asp.h"
#include "web.h"

extern int __libc_heap_used;
extern int __libc_heap_max;
extern int __libc_stack_max;

extern void(*__libc_ECS)(LPCRITICAL_SECTION lock);
extern void(*__libc_LCS)(LPCRITICAL_SECTION lock);

// Tell the library that the default 1023 rent table spaces isn't enough...


// MVS Conversion Tables: (ASCII to codepage-1047 back to ASCII)

unsigned char __ascii_to_ebcdic [] = {
    "\x00\x01\x02\x03\x37\x2D\x2E\x2F\x16\x05\x15\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x3C\x3D\x32\x26\x18\x19\x3F\x27\x1C\x1D\x1E\x1F"
    "\x40\x5A\x7F\x7B\x5B\x6C\x50\x7D\x4D\x5D\x5C\x4E\x6B\x60\x4B\x61\xF0\xF1\xF2\xF3\xF4\xF5\xF6\xF7\xF8\xF9\x7A\x5E\x4C\x7E\x6E\x6F"
    "\x7C\xC1\xC2\xC3\xC4\xC5\xC6\xC7\xC8\xC9\xD1\xD2\xD3\xD4\xD5\xD6\xD7\xD8\xD9\xE2\xE3\xE4\xE5\xE6\xE7\xE8\xE9\xAD\xE0\xBD\x5F\x6D"
    "\x79\x81\x82\x83\x84\x85\x86\x87\x88\x89\x91\x92\x93\x94\x95\x96\x97\x98\x99\xA2\xA3\xA4\xA5\xA6\xA7\xA8\xA9\xC0\x4F\xD0\xA1\x07"
    "\x20\x21\x22\x23\x24\x25\x06\x17\x28\x29\x2A\x2B\x2C\x09\x0A\x1B\x30\x31\x1A\x33\x34\x35\x36\x08\x38\x39\x3A\x3B\x04\x14\x3E\xFF"
    "\x41\xAA\x4A\xB1\x9F\xB2\x6A\xB5\xBB\xB4\x9A\x8A\xB0\xCA\xAF\xBC\x90\x8F\xEA\xFA\xBE\xA0\xB6\xB3\x9D\xDA\x9B\x8B\xB7\xB8\xB9\xAB"
    "\x64\x65\x62\x66\x63\x67\x9E\x68\x74\x71\x72\x73\x78\x75\x76\x77\xAC\x69\xED\xEE\xEB\xEF\xEC\xBF\x80\xFD\xFE\xFB\xFC\xBA\xAE\x59"
    "\x44\x45\x42\x46\x43\x47\x9C\x48\x54\x51\x52\x53\x58\x55\x56\x57\x8C\x49\xCD\xCE\xCB\xCF\xCC\xE1\x70\xDD\xDE\xDB\xDC\x8D\x8E\xDF"
};
unsigned char __ebcdic_to_ascii [] = {
    "\x00\x01\x02\x03\x9C\x09\x86\x7F\x97\x8D\x8E\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x9D\x0A\x08\x87\x18\x19\x92\x8F\x1C\x1D\x1E\x1F"
    "\x80\x81\x82\x83\x84\x85\x17\x1B\x88\x89\x8A\x8B\x8C\x05\x06\x07\x90\x91\x16\x93\x94\x95\x96\x04\x98\x99\x9A\x9B\x14\x15\x9E\x1A"
    "\x20\xA0\xE2\xE4\xE0\xE1\xE3\xE5\xE7\xF1\xA2\x2E\x3C\x28\x2B\x7C\x26\xE9\xEA\xEB\xE8\xED\xEE\xEF\xEC\xDF\x21\x24\x2A\x29\x3B\x5E"
    "\x2D\x2F\xC2\xC4\xC0\xC1\xC3\xC5\xC7\xD1\xA6\x2C\x25\x5F\x3E\x3F\xF8\xC9\xCA\xCB\xC8\xCD\xCE\xCF\xCC\x60\x3A\x23\x40\x27\x3D\x22"
    "\xD8\x61\x62\x63\x64\x65\x66\x67\x68\x69\xAB\xBB\xF0\xFD\xFE\xB1\xB0\x6A\x6B\x6C\x6D\x6E\x6F\x70\x71\x72\xAA\xBA\xE6\xB8\xC6\xA4"
    "\xB5\x7E\x73\x74\x75\x76\x77\x78\x79\x7A\xA1\xBF\xD0\x5B\xDE\xAE\xAC\xA3\xA5\xB7\xA9\xA7\xB6\xBC\xBD\xBE\xDD\xA8\xAF\x5D\xB4\xD7"
    "\x7B\x41\x42\x43\x44\x45\x46\x47\x48\x49\xAD\xF4\xF6\xF2\xF3\xF5\x7D\x4A\x4B\x4C\x4D\x4E\x4F\x50\x51\x52\xB9\xFB\xFC\xF9\xFA\xFF"
    "\x5C\xF7\x53\x54\x55\x56\x57\x58\x59\x5A\xB2\xD4\xD6\xD2\xD3\xD5\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\xB3\xDB\xDC\xD9\xDA\x9F"
};

long running = 1; // Give global access to any classes that are prelinked/dynamic
static long running2 = 1; // The Scan routine for Cookies/Sessions
WebSite_ptr wp;   // to these two variables.

// Cookie functions: (aka. Sessions)
//
// (And the Cookie-Jar!)

typedef struct cookie_jar * cookie_ptr;
typedef struct cookie_jar {
    char     * cookie_string;
    SESSION  * Session;
    cookie_ptr next;
} cookiejar;

static CRITICAL_SECTION cookie_lock = 0; // (Pre-Initialised 'shortcut')
       long             server_index = 0; // External.
static long long        cookie_index = 0;

static long             cpcnt  = 0;
static cookie_ptr       cphead = NULL;

static long             scan_tid = 0;   // Non-Zero when Scan_Cookie is running.

static void Scan_Cookie (void * a) { // Worker-Thread!
    cookie_ptr cp;
    cookie_ptr cpp;
    cookie_ptr cpt;
    time_t     it; // Interval-Time
    long       c = 0;

    while ((running) && (running2)) {

        Sleep (1000);

        if (++c >= 60) { // 1 Minute between scans:
            c = 0;

            it = time (NULL);

            EnterCriticalSection (&cookie_lock);

            cp = cphead;
            cpp = NULL;
            while (cp) {
                if (cp->Session->session_used_time + (60 * wp->session_t_o) < it) {

                    // Delete Session - timeout!
                    if (cpp == NULL)
                        cphead = cp->next;
                    else
                        cpp->next = cp->next;

                    cpt = cp;
                    cp = cp->next;

                    _del_object (cpt->Session);
                    free (cpt);

                    cpcnt--;

                } else {
                    cpp = cp;
                    cp = cp->next;
                };
            };

            LeaveCriticalSection (&cookie_lock);
        };
    };
};

static void Kill_Cookie () {
    cookie_ptr cp;
    cookie_ptr cpp = NULL;

    EnterCriticalSection (&cookie_lock);
    
    cp = cphead;
    while (cp) {
        cpp = cp;
        cp = cp->next;

        _del_object (cpp->Session);
        free (cpp);

        cpcnt--;
    };

    LeaveCriticalSection (&cookie_lock);
};

static SESSION * Get_Cookie (REQUEST * r) {
    SESSION * a = NULL;
    cookie_ptr cp;

    if (r->Cookie == NULL) return (NULL); // Don't have that one!

    EnterCriticalSection (&cookie_lock);

    cp = cphead;
    while (cp) {
        if (strcmp (cp->cookie_string, r->Cookie) == 0) {
            cp->Session->session_used_time = time (NULL);
            a = cp->Session;
            break;
        };
        cp = cp->next;
    };

    LeaveCriticalSection (&cookie_lock);

    return (a); // Not found!/Deleted!
};

char * New_Cookie (SESSION * s) {
    cookie_ptr cp;

    cp = (cookie_ptr)malloc (sizeof (cookiejar) + 48);
    if (cp) {
        cp->cookie_string = &(((char *)cp) [sizeof (cookiejar)]);

        EnterCriticalSection (&cookie_lock);
    
        sprintf (cp->cookie_string, "%08X=%016llX", server_index, cookie_index++);

        cp->Session = s;
        cp->next = cphead;
        cphead = cp;

        cpcnt++;

        LeaveCriticalSection (&cookie_lock);
    };

    if (cp)
        return (cp->cookie_string);
    else
        return (NULL);
};

// WebServer:

int HTMLSearchE (char * buffer, int length);

int HTMLEncodeE (char * b, int l) {
    char c1;
    long i;
    long j;
    long k;
    char * buf;

/*
* Implement This Here:
*
* '<' and '>' need to become &lt; &gt;
* '&' -> &amp;
* '"' -> &quot;
*/

    buf = (char *)malloc (l);
    memcpy (buf, b, l);
    i = 0;
    j = 0;
    do {
        i += (k = HTMLSearchE (&(buf [i]), l - i));
        if (k) {
            memcpy (&(b [j]), &(buf [i - k]), k);
            j += k;
        };
        if (i < l) {
            c1 = buf [i++];    // Skip this char.

            b [j++] = '&';
            switch (c1) {
            case '<':
                b [j++] = 'l';
                b [j++] = 't';
                break;
            case '>':
                b [j++] = 'g';
                b [j++] = 't';
                break;
            case '&':
                b [j++] = 'a';
                b [j++] = 'm';
                b [j++] = 'p';
                break;
            case '\"':
                b [j++] = 'q';
                b [j++] = 'u';
                b [j++] = 'o';
                b [j++] = 't';
                break;
            };
            b [j++] = ';';
        };
    } while (i < l);

    free (buf);

    return (j);
};

int URLSearchE (char * buffer, int length);

int URLEncodeE (char * b, int l, char * t) {
    char c1;
    char c2;
    long i;
    long j;
    long k;
    char buf [8193];

/*
*int URLEncodeE (char * buffer, int length, char * __ebcdic_to_ascii)
*Returns new length
*
*Unsafe: (' 'space) < > " # % { } | \ ^ ~ [ ] `
*- and a plus '+' becomes a space too.
*/

    memcpy (buf, b, l);
    i = 0;
    j = 0;
    do {
        i += (k = URLSearchE (&(buf [i]), l - i));
        if (k) {
            memcpy (&(b [j]), &(buf [i - k]), k);
            j += k;
        };
        if (i < l) {
            c1 = buf [i++];    // Skip this char.
            c2 = (c1 & 0x0F);
            c1 = (c1 >> 4) & 0x0F;

            b [j++] = '%';

            if (c1 <= 9)
                b [j++] = c1 + '0';
            else
                b [j++] = (c1 - 10) + 'A';

            if (c2 <= 9)
                b [j++] = c2 + '0';
            else
                b [j++] = (c2 - 10) + 'A';
        };
    } while (i < l);

    return (j);
};

/*** Scripts and their initialiser + run entry-points. ***/

static CRITICAL_SECTION script_lock = 0; // (Pre-Initialised 'shortcut')

Scripts_ptr shead = NULL;
static Scripts_ptr stail = NULL;

#define endmark "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
#define SKIP_MASK ((int) 0x1F)
#define ALIAS_MASK ((int) 0x80)

static void scan_directory () { // Read ASPSRCS for all TTRs which are in the shead list.
    long   i;
    long   j;
    FILE * fh;
    char   line [256];
    char   tstr [9];
    char * a;
    char * name;
    short  b;
    short  count;
    short  skip;
    long   quit;
    int    info_byte;
    short  l;
    Scripts_ptr s;

    fh = fopen (wp->user_sources, "rb,klen=0,lrecl=256,blksize=256,recfm=u,force");
    if (fh == NULL) return;

    EnterCriticalSection (&script_lock);

    fread (&l, 1, 2, fh); /* Skip U length */

    quit = 0;
    while (fread (line, 1, 256, fh) == 256) {

        a = &(line [2]);
        b = ((short *)&(line [0])) [0];
        count = 2;
        while (count < b) {
            
            if (memcmp (a, endmark, 8) == 0) {
                quit = 1;
                break;
            };

            name = a;
            a += 8;

            i = (((int *)a) [0]) & 0xFFFFFF00;
            a += 3;

            info_byte = (int)(*a);
            a++;

            skip = (info_byte & SKIP_MASK) * 2;
            
            strncpy (tstr, name, 8);
            j = 7;
            while (tstr [j] == ' ') j--;
            tstr [++j] = 0;

#ifndef admin_dyn
            if (stricmp (tstr, "admin") != 0) {
#endif
            s = shead;
            while (s) {
                if (stricmp (tstr, s->name) == 0) {
                    if (s->ttr != i) s->ttr = 0; // mark as reload-required.
                    break; // Keep looking at members...
                };
                s = s->next;
            };
#ifndef admin_dyn
            };
#endif

            a += skip;

            count += (8 + 4 + skip);
        };

        if (quit) break;

        fread (&l, 1, 2, fh); /* Skip U length */
    };

    LeaveCriticalSection (&script_lock);

    fclose (fh);
    return;
};

static void unload_Scripts () {
    Scripts_ptr p;
    Scripts_ptr s;

    EnterCriticalSection (&script_lock);

    s = shead;
    shead = NULL;
    stail = NULL;
    while (s) {

        _end_object (s->obj);
        _jcc_unload (s->dynobp);

        p = s;
        s = s->next;

        free (p);
    };

    LeaveCriticalSection (&script_lock);
};

static void del_Script2 (void * dyno) {
    Scripts_ptr p = NULL;
    Scripts_ptr s;

    if (dyno == NULL) return;

    EnterCriticalSection (&script_lock);

    s = shead;
    while (s) {
        if (s->dynobp == dyno) {
            if (p == NULL)
                shead = s->next;
            else
                p->next = s->next;

            _end_object (s->obj);
            _jcc_unload (dyno);

            if (s == stail) stail = p;

            free (s);
            
            LeaveCriticalSection (&script_lock);

            return;
        };
        p = s;
        s = s->next;
    };

    LeaveCriticalSection (&script_lock);
};

int del_Script (char * path, char * name, char * ext) {
    Scripts_ptr p = NULL;
    Scripts_ptr s;

    EnterCriticalSection (&script_lock);

    s = shead;
    while (s) {
        if ((stricmp (s->name, name) == 0) &&
            (stricmp (s->path, path) == 0) &&
            (stricmp (s->ext, ext) == 0)) {
            if (p == NULL)
                shead = s->next;
            else
                p->next = s->next;

            if (s->dynobp) {
                _end_object (s->obj);
                _jcc_unload (s->dynobp);
            };

            if (s == stail) stail = p;

            free (s);

            LeaveCriticalSection (&script_lock);

            return (1);
        };
        p = s;
        s = s->next;
    };

    LeaveCriticalSection (&script_lock);
    
    return (0);
};

int zap_Script (char * path, char * name, char * ext) {
    Scripts_ptr s;

    EnterCriticalSection (&script_lock);

    s = shead;
    while (s) {
        if ((stricmp (s->name, name) == 0) &&
            (stricmp (s->path, path) == 0) &&
            (stricmp (s->ext, ext) == 0)) {

            s->ttr = 0;

            LeaveCriticalSection (&script_lock);

            return (1);
        };
        s = s->next;
    };

    LeaveCriticalSection (&script_lock);
    
    return (0);
};

static int add_Script2 (char * path, char * name, char * ext, char * init, char * exit, char * arun, void * dyno, void * obj, void * run, long ttr) {
    Scripts_ptr s;
    long i;
    long j;
    long k;
    long l;
    long m;
    long n;

    n = strlen (path) + 1;
    m = strlen (name) + 1 + n;
    l = strlen (ext) + 1 + m;

    k = strlen (init) + 1 + l;
    j = strlen (exit) + 1 + k;
    i = strlen (run)  + 1 + j;

    s = (Scripts_ptr)malloc (sizeof (Scripts) + i);
    
    s->path = strcpy ((char *)&(((char *)s) [sizeof (Scripts)]), path);
    s->name = strcpy ((char *)&(((char *)s) [sizeof (Scripts) + n]), name);
    s->ext = strcpy ((char *)&(((char *)s) [sizeof (Scripts) + m]), ext);
    s->init = strcpy ((char *)&(((char *)s) [sizeof (Scripts) + l]), init);
    s->exit = strcpy ((char *)&(((char *)s) [sizeof (Scripts) + k]), exit);
    s->arun = strcpy ((char *)&(((char *)s) [sizeof (Scripts) + j]), arun);

    s->dynobp = dyno;
    s->obj = obj;

    s->run = run;
    s->ttr = ttr;

    s->next = NULL;

    EnterCriticalSection (&script_lock);

    if (stail) {
        stail->next = s;
    } else {
        shead = s;
    };
    stail = s;

    LeaveCriticalSection (&script_lock);

    return (1);
};

int add_Script (char * path, char * name, char * ext, char * init, char * exit, char * arun) {
    Scripts_ptr s;

    s = shead;
    while (s) {
        if ((stricmp (s->name, name) == 0) &&
            (stricmp (s->path, path) == 0) &&
            (stricmp (s->ext, ext) == 0))
            return (0);
        s = s->next;
    };

    return (add_Script2 (path, name, ext, init, exit, arun, NULL, NULL, NULL, 0));
};

static Scripts_ptr get_Script (char * path, char * name, char * ext) {
    Scripts_ptr s;

    s = shead;
    while (s) {
        if ((stricmp (s->name, name) == 0) &&
            (stricmp (s->path, path) == 0) &&
            (stricmp (s->ext, ext) == 0))
            return (s);
        s = s->next;
    };

    return (NULL);
};

/*** Initialisation of prelinked (prebuilt) class objects ***/

static void start_objs () {

    _jcc_object (String_init, String_exit);
    _jcc_object (Variant_init, Variant_exit);

    _jcc_object (VBError_init, VBError_exit);

    _jcc_object (Collection_init, Collection_exit);

    _jcc_object (ObjectContext_init, ObjectContext_exit);
    _jcc_object (Response_init, Response_exit);
    _jcc_object (Request_init, Request_exit);
    _jcc_object (Session_init, Session_exit);

    _jcc_object (Server_init, Server_exit);
    _jcc_object (Application_init, Application_exit);
};

static void stop_objs () {

    // End-Order is significant...
    _end_object (Application_init);
    _end_object (Server_init);

    _end_object (Session_init);
    _end_object (Request_init);
    _end_object (Response_init);
    _end_object (ObjectContext_init);

    _end_object (Collection_init);

    _end_object (VBError_init);

    _end_object (Variant_init);
    _end_object (String_init);
};

/*** Save and Load ***/

static void wdel (WebSite_ptr wp) {

    free (wp->user_sources);
    free (wp->user_headers);
    free (wp->jccl_headers);
    free (wp->name);
    free (wp->description);
    free (wp->open_card);
    free (wp->logdir);
    free (wp);
};

static void save (char * filename, WebSite_ptr w) {
    FILE * fh;
    long   i;
    Scripts_ptr s;

    fh = fopen (filename, "wb");
    if (fh) {
        fwrite ("JCC-ASP1.0", 1, 10, fh);

        fwrite (&(cookie_index), 1, 8, fh);
        fwrite (&(server_index), 1, 4, fh);

        i = strlen (w->user_sources);
        fwrite (&(i), 1, 4, fh);
        fwrite (w->user_sources, 1, i, fh);

        i = strlen (w->user_headers);
        fwrite (&(i), 1, 4, fh);
        fwrite (w->user_headers, 1, i, fh);

        i = strlen (w->jccl_headers);
        fwrite (&(i), 1, 4, fh);
        fwrite (w->jccl_headers, 1, i, fh);

        i = strlen (w->name);
        fwrite (&(i), 1, 4, fh);
        fwrite (w->name, 1, i, fh);

        i = strlen (w->description);
        fwrite (&(i), 1, 4, fh);
        fwrite (w->description, 1, i, fh);
        
        i = strlen (w->open_card);
        fwrite (&(i), 1, 4, fh);
        fwrite (w->open_card, 1, i, fh);

        fwrite (&(w->open_port), 1, 4, fh);
        fwrite (&(w->worker_threads), 1, 4, fh);
        fwrite (&(w->sessions_max), 1, 4, fh);
        fwrite (&(w->session_t_o), 1, 4, fh);
        fwrite (&(w->logging), 1, 4, fh);
        fwrite (&(w->logswitch), 1, 4, fh);
        
        i = strlen (w->logdir);
        fwrite (&(i), 1, 4, fh);
        fwrite (w->logdir, 1, i, fh);

        fwrite (&(w->logopts), 1, 4, fh);

        s = shead;
        fwrite (&s, 1, 4, fh);
        while (s) {

            i = strlen (s->path);
            fwrite (&(i), 1, 4, fh);
            fwrite (s->path, 1, i, fh);

            i = strlen (s->name);
            fwrite (&(i), 1, 4, fh);
            fwrite (s->name, 1, i, fh);

            i = strlen (s->ext);
            fwrite (&(i), 1, 4, fh);
            fwrite (s->ext, 1, i, fh);

            i = strlen (s->init);
            fwrite (&(i), 1, 4, fh);
            fwrite (s->init, 1, i, fh);

            i = strlen (s->exit);
            fwrite (&(i), 1, 4, fh);
            fwrite (s->exit, 1, i, fh);

            i = strlen (s->arun);
            fwrite (&(i), 1, 4, fh);
            fwrite (s->arun, 1, i, fh);

            s = s->next;
            fwrite (&s, 1, 4, fh);
        };

        fclose (fh);
    };
};

static WebSite_ptr load (char * filename) {
    FILE * fh;
    WebSite_ptr wp;
    char csum [10];
    long i;
    char path [260];
    char name [260];
    char ext  [260];
    char init [260];
    char exit [260];
    char arun [260];

    wp = (WebSite_ptr)malloc (sizeof (WebSite));

    wp->save_trigger = 0; // No Save Needed (yet.)

    csum [0] = 0;
    fh = fopen (filename, "rb");
    if (fh) {
        if (((i = fread (csum, 1, 10, fh)) != 10) || (memcmp ("JCC-ASP1.0", csum, 10) != 0)) {
            fclose (fh); // oops! this isn't our file...
            csum [0] = i; // Overwrite it?
            goto default_settings;
        };
        wp->save_disable = 0;

        fread (&(cookie_index), 1, 8, fh);
        fread (&(server_index), 1, 4, fh);

        fread (&(i), 1, 4, fh);
        wp->user_sources = (char *)malloc (i + 1);
        fread (wp->user_sources, 1, i, fh);
        wp->user_sources [i] = 0;

        fread (&(i), 1, 4, fh);
        wp->user_headers = (char *)malloc (i + 1);
        fread (wp->user_headers, 1, i, fh);
        wp->user_headers [i] = 0;

        fread (&(i), 1, 4, fh);
        wp->jccl_headers = (char *)malloc (i + 1);
        fread (wp->jccl_headers, 1, i, fh);
        wp->jccl_headers [i] = 0;

        fread (&(i), 1, 4, fh);
        wp->name = (char *)malloc (i + 1);
        fread (wp->name, 1, i, fh);
        wp->name [i] = 0;

        fread (&(i), 1, 4, fh);
        wp->description = (char *)malloc (i + 1);
        fread (wp->description, 1, i, fh);
        wp->description [i] = 0;

        fread (&(i), 1, 4, fh);
        wp->open_card = (char *)malloc (i + 1);
        fread (wp->open_card, 1, i, fh);
        wp->open_card [i] = 0;

        fread (&(wp->open_port), 1, 4, fh);
        fread (&(wp->worker_threads), 1, 4, fh);
        fread (&(wp->sessions_max), 1, 4, fh);
        fread (&(wp->session_t_o), 1, 4, fh);
        fread (&(wp->logging), 1, 4, fh);
        fread (&(wp->logswitch), 1, 4, fh);

        fread (&(i), 1, 4, fh);
        wp->logdir = (char *)malloc (i + 1);
        fread (wp->logdir, 1, i, fh);
        wp->logdir [i] = 0;

        fread (&(wp->logopts), 1, 4, fh);

        fread (&i, 1, 4, fh);
        while (i) {

            fread (&(i), 1, 4, fh);
            fread (path, 1, i, fh);
            path [i] = 0;

            fread (&(i), 1, 4, fh);
            fread (name, 1, i, fh);
            name [i] = 0;

            fread (&(i), 1, 4, fh);
            fread (ext, 1, i, fh);
            ext [i] = 0;

            fread (&(i), 1, 4, fh);
            fread (init, 1, i, fh);
            init [i] = 0;

            fread (&(i), 1, 4, fh);
            fread (exit, 1, i, fh);
            exit [i] = 0;

            fread (&(i), 1, 4, fh);
            fread (arun, 1, i, fh);
            arun [i] = 0;

#ifndef admin_dyn
            if ((stricmp (path, "/") != 0) ||
                (stricmp (name, "admin") != 0) ||
                (stricmp (ext, "asp") != 0))
#endif
            add_Script2 (path, name, ext, init, exit, arun, NULL, NULL, NULL, 0);

            fread (&i, 1, 4, fh);
        };

        fclose (fh);

    } else { // Provide a default one...

default_settings:
        wp->save_disable = csum [0];

        wp->user_sources = (char *)malloc (14);
        strcpy (wp->user_sources, "//DDN:ASPSRCS");
        wp->user_headers = (char *)malloc (14);
        strcpy (wp->user_headers, "//DDN:JCCINCS");
        wp->jccl_headers = (char *)malloc (14);
        strcpy (wp->jccl_headers, "//DDN:JCCINCL");

        wp->name = (char *)malloc (10);
        strcpy (wp->name, "127.0.0.1");

        wp->description = (char *)malloc (17);
        strcpy (wp->description, "Default Web Site");

        wp->open_card = (char *)malloc (4);
        strcpy (wp->open_card, "any");

        wp->open_port = default_port;
        wp->worker_threads = 0;

        wp->sessions_max = 0; // unlimited.
        wp->session_t_o = 20; // 20 minutes for default timeout.

        wp->logging = 0;

        wp->logswitch = 0; // Use sequential file
        wp->logdir = (char *)malloc (13);
        strcpy (wp->logdir, "//DDN:LOGDIR");
        wp->logopts = 0x60D00000;
    };

    if (wp->worker_threads > MaxThreads)
        wp->worker_threads = MaxThreads;
    if (wp->worker_threads < 0)
        wp->worker_threads = 0;

#ifdef DEBUG
    wp->save_disable = 1;
    wp->logging = 1;
    wp->logopts = 0xFFFFFFFF;
#endif

    return (wp);
};

/*** Multi-Threaded Server ***/

static long             handles  [MaxThreads + 1];
static int              owner    [MaxThreads + 1];
static void           * ups      [MaxThreads + 1][8];
static CRITICAL_SECTION locks    [MaxThreads + 1]; // These locks are owned by both the worker_thread
static CRITICAL_SECTION termlok1 [MaxThreads + 1]; // and the master-thread, and may be passed to one-
static CRITICAL_SECTION termlok2 [MaxThreads + 1]; // another even when in a locked state! (JCC-Permits.)

static int              threadsig = 0; // Signal a specific thread to end. (0=master, and can't end.)

static long             Log_Err  = 0; // Stop trying to log?
static CRITICAL_SECTION Log_Lock = 0;
static FILE * loghandle = NULL;

typedef struct LogE_tag * LogE_ptr;
typedef struct LogE_tag {
    char   * entry;
    LogE_ptr next;
} LogE;

LogE_ptr Lhead = NULL;
LogE_ptr Ltail = NULL; // Must be inserted at the end!

static const char * log_props [] = {
    "date",
    "time",
    "c-ip",
    "user",
    "svc",
    "svr",
    "s-ip",
    "s-pt",
    "cs-method",
    "cs-uri-stem",
    "cs-uri-query",
    "sc-status",
    "send-bytes",
    "recv-bytes",
    "time",
    "proto",
    "host",
    "u-agent",
    "cookie",
    "referer"
};

static void FlushLog () {
    long i;
    long j;
    char buf [256];
    struct tm * today;
    LogE_ptr Lp;
    LogE_ptr Lpp;

    if ((Log_Err) || (Lhead == NULL)) return;

    EnterCriticalSection (&Log_Lock);

    // Open the file, if it needs to be...
    if (loghandle == NULL) {

        loghandle = fopen (wp->logdir, "a");

        if (loghandle == NULL) {
            Log_Err = 1;
            return;
        };

        fprintf (loghandle, "#Software: JCC-ASP1.0\n");
        fprintf (loghandle, "#Version: 1.0\n");

        i = time (NULL);
        today = localtime (&i);
        strftime (buf, 256, "%Y-%m-%d %H:%M:%S", today);
    
        fprintf (loghandle, "#Date: %s\n", buf);
        fprintf (loghandle, "#Fields:");

        // loop
        i = 0;
        j = wp->logopts;
        while (i < 20) {
            if (j & 0x80000000)
                fprintf (loghandle, " %s", log_props [i]);
            j <<= 1;
            i++;
        };
        fprintf (loghandle, "\n");
    };

    Lp = Lhead;
    while (Lp) {

        fprintf (loghandle, "%s", Lp->entry);

        Lpp = Lp;
        Lp = Lp->next;

        free (Lpp);
    };

    Lhead = NULL;
    Ltail = NULL;

    LeaveCriticalSection (&Log_Lock);
};

static void DoLogging (REQUEST * rp, RESPONS * pp, long ms) {
    long i;
    long j;
    long f;
    char buf [8192];
    struct tm * today;
    LogE_ptr Lp;
    COLLECT * c;
    VARIANT * v;

    if (Log_Err) return;

    pp->Flush (); // Ensure all logged details are up-to-date...

    // Log the required properties...
    j = wp->logopts;
    i = 0;
    if (j & 0x80000000) {          // Date...
        today = localtime (&rp->start_time);
        if ((j & 0x40000000) == 0) // Date & Time
            i = strftime (buf, 256, "%Y-%m-%d", today) - 1;
        else                       // Date only
            i = strftime (buf, 256, "%Y-%m-%d %H:%M:%S", today) - 1;

    } else if (j & 0x40000000) {   // Time only
        today = localtime (&rp->start_time);
        i = strftime (buf, 256, "%H:%M:%S", today) - 1;
    } else
        i = 0;

    if (j & 0x20000000) { // Client IP Address

        if (i) i += sprintf (&(buf [i]), " ");
        i += sprintf (&(buf [i]), "%d.%d.%d.%d",
                       (rp->client_ip & 0xFF000000) >> 24,
                       (rp->client_ip & 0x00FF0000) >> 16,
                       (rp->client_ip & 0x0000FF00) >> 8,
                        rp->client_ip & 0x000000FF);
    };

    if (j & 0x10000000) { // User Name
        if (i) i += sprintf (&(buf [i]), " ");

        c = rp->ServerVariables;
//        v = c->Default ("AUTH_USER");
        v = c->Default ("REMOTE_USER");
        if ((v) && (v->SubType == 1))
            i += sprintf (&(buf [i]), "%s", ((STRING *)(v->Value.ov))->Value);
        else
            i += sprintf (&(buf [i]), "no-user");
    };

    if (j & 0x08000000) { // Service Name
        if (i) i += sprintf (&(buf [i]), " ");
        i += sprintf (&(buf [i]), "%s", wp->description);
    };

    if (j & 0x04000000) { // Server Name
        if (i) i += sprintf (&(buf [i]), " ");
        i += sprintf (&(buf [i]), "%s", wp->name);
    };

    if (j & 0x02000000) { // Server IP Address

        if (i) i += sprintf (&(buf [i]), " ");
        i += sprintf (&(buf [i]), "%d.%d.%d.%d",
                       (rp->host_ip & 0xFF000000) >> 24,
                       (rp->host_ip & 0x00FF0000) >> 16,
                       (rp->host_ip & 0x0000FF00) >> 8,
                        rp->host_ip & 0x000000FF);
    };

    if (j & 0x01000000) { // Server Port
        if (i) i += sprintf (&(buf [i]), " ");
        i += sprintf (&(buf [i]), "%d", wp->open_port);
    };

    if (j & 0x00800000) { // Method
        if (i) i += sprintf (&(buf [i]), " ");
        if (rp->post)
            i += sprintf (&(buf [i]), "POST");
        else
            i += sprintf (&(buf [i]), "GET");
    };

    if (j & 0x00400000) { // URI Stem
        if (i) i += sprintf (&(buf [i]), " ");
        i += sprintf (&(buf [i]), "%s%s.%s", rp->Path, rp->File, rp->Ext);
    };

    if (j & 0x00200000) { // URI Query
        if (i) i += sprintf (&(buf [i]), " ");
        if (rp->qs)
            i += sprintf (&(buf [i]), "%s", rp->qs);
        else
            i += sprintf (&(buf [i]), "no-querystring");
    };

    if (j & 0x00100000) { // Protocol Status
        if (i) i += sprintf (&(buf [i]), " ");
        i += sprintf (&(buf [i]), "%d", pp->send_code);
    };

    if (j & 0x00080000) { // Bytes Sent
        if (i) i += sprintf (&(buf [i]), " ");
        i += sprintf (&(buf [i]), "%d", pp->TotalBytes);
    };

    if (j & 0x00040000) { // Bytes Received
        if (i) i += sprintf (&(buf [i]), " ");
        i += sprintf (&(buf [i]), "%d", rp->TotalBytes);
    };

    if (j & 0x00020000) { // Time Taken
        if (i) i += sprintf (&(buf [i]), " ");
        i += sprintf (&(buf [i]), "%dms", ms);
    };

    if (j & 0x00010000) { // Protocol Version
        if (i) i += sprintf (&(buf [i]), " ");
        i += sprintf (&(buf [i]), "HTTP/1.1"); // *** This should check.
    };

    if (j & 0x00008000) { // Host
        if (i) i += sprintf (&(buf [i]), " ");

        c = rp->ServerVariables;
        v = c->Default ("HOST");
        if ((v) && (v->SubType == 1))
            i += sprintf (&(buf [i]), "%s", ((STRING *)(v->Value.ov))->Value);
        else
            i += sprintf (&(buf [i]), "no-host");
    };

    if (j & 0x00004000) { // User Agent
        if (i) i += sprintf (&(buf [i]), " ");


        c = rp->ServerVariables;
        v = c->Default ("USER_AGENT");
        if ((v) && (v->SubType == 1))
            i += sprintf (&(buf [i]), "%s", ((STRING *)(v->Value.ov))->Value);
        else
            i += sprintf (&(buf [i]), "no-useragent");
    };

    if (j & 0x00002000) { // Cookie
        if (i) i += sprintf (&(buf [i]), " ");
        if (rp->Cookie)
            i += sprintf (&(buf [i]), "%s", rp->Cookie);
        else
            i += sprintf (&(buf [i]), "no-cookie");
    };

    if (j & 0x00001000) { // Referer
        if (i) i += sprintf (&(buf [i]), " ");

        c = rp->ServerVariables;
        v = c->Default ("REFERER");
        if ((v) && (v->SubType == 1))
            i += sprintf (&(buf [i]), "%s", ((STRING *)(v->Value.ov))->Value);
        else
            i += sprintf (&(buf [i]), "no-referer");
    };

    i += sprintf (&(buf [i]), "\n"); // Entry Complete

    Lp = (LogE_ptr)malloc (sizeof (LogE) + i + 1);
    Lp->entry = &(((char *)Lp) [sizeof (LogE)]);
    memcpy (Lp->entry, buf, i + 1);
    Lp->next = NULL;

    EnterCriticalSection (&Log_Lock);

    if (Lhead == NULL)
        Lhead = Lp;
    else
        Ltail->next = Lp;
    Ltail = Lp;

    LeaveCriticalSection (&Log_Lock);
};

static void worker_thread (long i) {
    long      j;
    VBERROR * ErrorInstance;

    while (1) {
        
        EnterCriticalSection (&(locks [i]));
        LeaveCriticalSection (&(termlok1 [i]));
        EnterCriticalSection (&(termlok2 [i]));

        if ((threadsig == i) && (owner [i] == 1)) {
            LeaveCriticalSection (&(locks [i]));
            return; // Exit signal
        };

        //
        // Do some web processing
        //

        if ((wp->logging) && (wp->logopts))
            j = clock ();

        ErrorInstance = _new_object (VBError_init);
        ErrorInstance->Response = ups [i][5];
        _push_object (ErrorInstance);

        _push_object (ups [i][0]); // Enable new-c_Wrap
        (*((void (*)())(ups [i][7]))) (ups [i][0], ups [i][1], ups [i][2], ups [i][3], ups [i][4], ups [i][5], ups [i][6], ErrorInstance);
        _pop_object (ups [i][0]);  // Disable again now

        _pop_object (ErrorInstance);
        _del_object (ErrorInstance);

        if ((wp->logging) && (wp->logopts)) {
            j = clock () - j; // Get milliseconds past
            DoLogging (ups [i][4], ups [i][5], j);
        };

        _del_object (ups [i][6]);
        _del_object (ups [i][5]);
        _del_object (ups [i][4]);
        _del_object (ups [i][0]);

        //
        // Back to normal
        //
        
        EnterCriticalSection (&(termlok1 [i]));
        owner [i] = 1;
        LeaveCriticalSection (&(locks [i]));
        LeaveCriticalSection (&(termlok1 [i]));
        EnterCriticalSection (&(termlok2 [i]));
    };
};

static int started_threads = 0;
static int rrobin = 1;

static void rectify_threads (WebSite_ptr wp) {
    long i;
    long old_started_threads = started_threads;

    if (started_threads > wp->worker_threads) {
        while (started_threads > wp->worker_threads) {
            threadsig = started_threads;
            
            if (owner [started_threads] == 0) {
                LeaveCriticalSection (&(termlok1 [started_threads]));
                EnterCriticalSection (&(locks [started_threads]));
                EnterCriticalSection (&(termlok1 [started_threads]));
                LeaveCriticalSection (&(termlok2 [started_threads]));
            };
            
            LeaveCriticalSection (&(locks [started_threads]));
            EnterCriticalSection (&(termlok1 [started_threads]));
            LeaveCriticalSection (&(termlok2 [started_threads]));
            syncthread (handles [started_threads]);

            started_threads--;
        };
        threadsig = 0;
    } else {
        while (started_threads < wp->worker_threads) { // Start worker_threads
            started_threads++;
            owner [started_threads] = 1;
            EnterCriticalSection (&(locks [started_threads]));
            EnterCriticalSection (&(termlok1 [started_threads]));
            EnterCriticalSection (&(termlok2 [started_threads]));

            i = beginthread ((void *)&worker_thread, 0, (void *)started_threads);
            if (i == -1) {
                started_threads--;
                goto rt_finished;
//                return; // Error: can't create anymore!
            };

            handles [started_threads] = i;
        };
    };

rt_finished:
    if (old_started_threads == 0) {
        if (started_threads != 0) { // Start the Session/Cookie Scanner.
            running2 = 1;
            scan_tid = beginthread ((void *)&Scan_Cookie, 0, (void *)0);
//        } else {                  // Do Nothing... Nothing Changed.
        };
    } else {
        if (started_threads == 0) { // Stop the Session/Cookie Scanner.
            running2 = 0;
            if (scan_tid != -1)
                syncthread (scan_tid);
//        } else {                  // Do Nothing... Threads are still going.
        };
    };
};

static void flush_threads (WebSite_ptr wp) {
    long i;

    i = 1;
    while (started_threads >= i) {

        if (owner [started_threads] == 0) {
            LeaveCriticalSection (&(termlok1 [started_threads]));
            EnterCriticalSection (&(locks [started_threads]));
            EnterCriticalSection (&(termlok1 [started_threads]));
            LeaveCriticalSection (&(termlok2 [started_threads]));
        };
        i++;
    };
};

static void launch_next (void * f, void * up, SERVER * sp, APPLICA * ap, SESSION * np, REQUEST * rp, RESPONS * pp, OBJECTC * op) {
    VBERROR  * ErrorInstance;
    long       i;
    cookie_ptr cp;
    cookie_ptr cpp;
    cookie_ptr cpt;

    if (started_threads == 0) {
        
        //
        // Do some web processing
        //

        if ((wp->logging) && (wp->logopts))
            i = clock ();

        ErrorInstance = _new_object (VBError_init);
        ErrorInstance->Response = pp;
        _push_object (ErrorInstance);

        _push_object (up); // Enable new-c_Wrap
        (*((void (*)())f)) (up, sp, ap, np, rp, pp, op, ErrorInstance);
        _pop_object (up);  // Disable again now

        _pop_object (ErrorInstance);
        _del_object (ErrorInstance);
    
        if ((wp->logging) && (wp->logopts)) {
            i = clock () - i; // Get milliseconds past
            DoLogging (rp, pp, i);
        };

        _del_object (op);
        _del_object (pp);
        _del_object (rp);
        _del_object (up);

        // Check that the Session isn't marked for deletion...
        if (np->session_used_time == 0) {

            cp = cphead;
            while (cp) {
                if (cp->Session == np) { // Find and remove it...

                    // Delete Session - timeout!
                    if (cpp == NULL)
                        cphead = cp->next;
                    else
                        cpp->next = cp->next;

                    cpt = cp;
                    cp = cp->next;

                    _del_object (cpt->Session);
                    free (cpt);

                    cpcnt--;

                } else {
                    cpp = cp;
                    cp = cp->next;
                };
            };
        };

        //
        // Back to normal
        //

        return;
    };

    if (owner [rrobin] == 0) {
        
        i = 1;
        while (i <= started_threads) {
            if (owner [i] == 1) {
                rrobin = i;
                break;
            };
            i++;
        };

        if (owner [rrobin] == 0) { // Nothing free, latch onto last-used thread.
            LeaveCriticalSection (&(termlok1 [rrobin]));
            EnterCriticalSection (&(locks [rrobin]));
            EnterCriticalSection (&(termlok1 [rrobin]));
            LeaveCriticalSection (&(termlok2 [rrobin]));
        };
    };

    ups [rrobin][0] = up;
    ups [rrobin][1] = sp;
    ups [rrobin][2] = ap;
    ups [rrobin][3] = np;
    ups [rrobin][4] = rp;
    ups [rrobin][5] = pp;
    ups [rrobin][6] = op;
    ups [rrobin][7] = f;
    owner [rrobin] = 0;

    LeaveCriticalSection (&(locks [rrobin]));
    EnterCriticalSection (&(termlok1 [rrobin]));
    LeaveCriticalSection (&(termlok2 [rrobin]));

    rrobin++;
    if (rrobin > started_threads) rrobin = 1;
};

/********************************************************************************************/
/********************************************************************************************/
/********************************************************************************************/

// dynamic reloader:

static int  reload_init = 0;
static char tmp1_jcc_dd [9]; // The Object once finished properly.
static char tmp2_jcc_dd [9]; // The precompiler output.

char tmp3_jcc_dd [9]; // The Errors/Messages.

static CRITICAL_SECTION reload_lock = 0;

void enter_reload () {
    EnterCriticalSection (&reload_lock);
};

void exit_reload () {
    LeaveCriticalSection (&reload_lock);
};

static int reload_compile (char * f) { // From Source to Object & Ready to Load...
    long   ttr = 1;
    long   r;
    FILE * fh;
    char   path [260];
    char   csect [9];
    char   filen [9];

    if (reload_init == 0) {
        reload_init = 1; // temp files (and their internal DDs) are only deleted on programs-end.
                         // *this program* and so don't need to do this again:
#ifdef MVS
        // Set up TEMP DDs for this entire session...
        // TMPJC1: An Object Output File
        fh = fopen ("//DSN:&&TMPJC1", "wb,recfm=fb,lrecl=80,blksize=12000,unit=sysda,pri=50");

        // TMPJC1: A temp output file to replace JCCOASM  DD object/asm sequential file.
        // Should get "JC1TP000" -> "999" (See start of main for 'JC1' part of TP000...)
        if (__get_tmpddn ("&&TMPJC1", tmp1_jcc_dd) == 0) {

            printf ("Abort in progress...  TMP file initialisation failed.\n");
            exit (8);
        };
        fclose (fh);

        // TMPJC2: A temp output file to replace JCCOUTPT DD -tempdir= default sequential file.
        fh = fopen ("//DSN:&&TMPJC2", "w,recfm=vb,lrecl=11996,blksize=12000,unit=sysda,pri=50");
        if (__get_tmpddn ("&&TMPJC2", tmp2_jcc_dd) == 0) {

            printf ("Abort in progress...  TMP file initialisation failed.\n");
            exit (8);
        };
        fclose (fh);

        // TMPJC3: STDOUT
        fh = fopen ("//DSN:&&TMPJC3", "w,recfm=fb,lrecl=80,blksize=12000,unit=sysda,pri=15");
        if (__get_tmpddn ("&&TMPJC3", tmp3_jcc_dd) == 0) {

            printf ("Abort in progress...  TMP file initialisation failed.\n");
            exit (8);
        };
        fclose (fh);
#else
        strcpy (tmp1_jcc_dd, "ASPTEMP1");
        strcpy (tmp2_jcc_dd, "ASPTEMP2");
        strcpy (tmp3_jcc_dd, "ASPTEMP3");
#endif
    };

    sprintf (path, "%s(%s)", wp->user_sources, f);
    fh = fopen (path, "r");
    if (fh) {
        ttr = __bldl (_fileno (fh)) & 0xFFFFFF00;

        fread (path, 1, 10, fh);
        
        fclose (fh);
    } else {
        fh = fopen (tmp3_jcc_dd, "w"); // Write our own message...
        if (fh) {
            fprintf (fh, "The Source Member %s(%s) cannot be opened.\n", wp->user_sources, f);
            fclose (fh);
        };
        return (0); // Can't access the file!
    };

    strncpy (csect, f, 7);
    csect [7] = 0;
    strupr (csect);
    strncpy (filen, f, 8);
    filen [8] = 0;
    strupr (filen);
    sprintf (path, "JCC -hfiles=%s -I%s -out=%s -tempdir=%s -fcode=@%s -renteprl %s(%s) >%s", wp->user_headers, wp->jccl_headers, tmp1_jcc_dd, tmp2_jcc_dd, csect, wp->user_sources, filen, tmp3_jcc_dd);

// Assumed JCL for system: (initial load only - until //DSN: style is used to prevent total locking.
//
// ASPSRCS  DD All Application Source Code.
// JCCINCS  DD Current-Dir User-Includes.
// JCCINCL  DD JCC Library Include Files.
//
// Note:
// Although all sources are in one place, each one can contain a different virtial-directory location within the
// actual web server which allows access to the script.

    r = system (path);
    if (r != 0)
        return (0);

    return (ttr);
};

void * reload (void * oo, char * path, char * file, char * ext, char * init, char * exit, char * run) {
    long   i;
    long   l1;
    long   l2;
    long   l3;
    void * a;
    char * b;
    FILE * fh;
    void * User_init;
    void * User_exit;
    void * ASP_run;

    if ((i = reload_compile (file)) != 0) {// Make ERROR Object!

        a = _jcc_load (tmp1_jcc_dd); // Load ERROR Object!

        if (a == NULL) {

            fh = fopen (tmp3_jcc_dd, "a"); // Write our own message...
            if (fh) {
                if (errno == EADDRNOTAVAIL)
                    fprintf (fh, "Couldn't reference symbol '%s' in loaded library.\n", _jcc_load_myref);
                else if (errno == ENOENT)
                    fprintf (fh, "Couldn't load Compiled Object from file.\n");
                else if (errno == EINVAL)
                    fprintf (fh, "Compiled Object is corrupt.\n");
                else
                    fprintf (fh, "The library cannot load anymore Objects.\n");

                fclose (fh);
            };

            return (oo);

        } else {

            User_init = _jcc_deref (init);
            if (User_init == NULL) {
                fh = fopen (tmp3_jcc_dd, "a"); // Write our own message...
                if (fh) {
                    fprintf (fh, "Couldn't dereference Object_init: '%s'\n", init);
                    fclose (fh);
                };
                _jcc_unload (a);
                return (oo);
            };

            User_exit = _jcc_deref (exit);
            if (User_exit == NULL) {
                fh = fopen (tmp3_jcc_dd, "a"); // Write our own message...
                if (fh) {
                    fprintf (fh, "Couldn't dereference Object_exit: '%s'\n", exit);
                    fclose (fh);
                };
                _jcc_unload (a);
                return (oo);
            };

            ASP_run = _jcc_deref (run);
            if (ASP_run == NULL) {
                fh = fopen (tmp3_jcc_dd, "a"); // Write our own message...
                if (fh) {
                    fprintf (fh, "Couldn't dereference Object_run: '%s'\n", run);
                    fclose (fh);
                };
                _jcc_unload (a);
                return (oo);
            };

            if (_jcc_object (User_init, User_exit) == 0) {
                fh = fopen (tmp3_jcc_dd, "a"); // Write our own message...
                if (fh) {
                    fprintf (fh, "Couldn't register New-Object\n");
                    fclose (fh);
                };
                _jcc_unload (a);
                return (oo);
            };

            if (oo == NULL) { // Special case, should never really happen...

                l1 = strlen (init) + 1;
                l2 = strlen (exit) + l1 + 1;
                l3 = strlen (run) + l2 + 1;
                b = (char *)malloc (l3);
                strcpy (b, init);
                strcpy (&(b [l1]), exit);
                strcpy (&(b [l2]), run);

                del_Script (path, file, ext);
                add_Script2 (path, file, ext, b, &(b [l1]), &(b [l2]), a, User_init, ASP_run, i);

            } else
                add_Script2 (path, file, ext, init, exit, run, a, User_init, ASP_run, i);

            if (oo) // Free the memory owned by the old object...
                del_Script2 (oo);

            fh = fopen (tmp3_jcc_dd, "a"); // Write our own message...
            if (fh) {
                fprintf (fh, "Possible Error: init/exit/run names must exist within the script.\n");
                fclose (fh);
            };

            return (a);
        };

    } else {

        return (oo);
    };
};

// main:

// The startup parameters will be read from the ini_file, then
// main will start a web server port (open_port = default_port)
// and allow connections.  Each new connection request is parsed
// for cookies (one is generated if not-present) and a Session
// is started - only if the page-request is a known application.

// There needs to be a limit to the number of connections - and
// it needs to be based on the size of the machine's memory which
// is 16MB in the case of MVS3.8j - gracefully reject new requests.
// (it would be silly to allow one new user to crash every one else.)

//
// On that note: currently the protection-buffer for Response are set to 128KB
// - and of course with 64 threads (max.) that's = 8MBs.
// - running with more than 0 worker-threads is just silly on a single
//   CPU machine too.  But Sessions will not timeout without a worker-thread.
// - 0 worker-threads is a valid setting and will out-perform the multi-thread
//   model on a single CPU machine such as Hercules/MVS3.8j and a Pentium.
//

//
// A note to the unwary...  Don't try to store an open FILE * in the users
// Session hoping it to work when you enable threads!  It will not, MVS only
// lets you use files from within the thread that opened it, and multi-threading
// doesn't make a determination for threads which serve any particular request.
//

int main (int argc, char * argv []) {
    long        i;
    long        j;
    long        r = 0; // Return Code
    APPLICA   * ap; // Global data (directory-sense)
    SERVER    * sp; // True Global data & stats.
    void      * up; // The execution engine
    void      * upd;// The default response execution engine
    SESSION   * np; // The default-data for execution
    SESSION   * np2;// The users-data for execution
    REQUEST   * rp; // A Request pointer
    RESPONS   * pp; // A Response pointer
    OBJECTC   * op; // An ObjectContext pointer
    long        initmem = __libc_heap_used;
    Scripts_ptr script;
    Scripts_ptr scriptd;
    long        default_used;
    long        old_workers;
    long        old_port;
    SOCKADDR_IN S_adx;
    SOCKET      serv_soc;
    SOCKET      soc;
    void      * error;
    void      * o;
    long        serv_ip;
    FILE      * fh;
    char        line [82];
    long        this_time;
    long        last_time = 0;
    long                 sz_sa = sizeof (struct sockaddr);
    struct sockaddr      sa;
    struct sockaddr_in * sap = (struct sockaddr_in *)&sa;

    wp = load (ini_file);

    // Check for overrides:

    i = 1;
    while (i < argc) {
        
        if (strncmp (argv [i], "-port=", 6) == 0) {
            
            wp->open_port = atoi (&(argv [i][6]));

        } else if (strncmp (argv [i], "-log=", 5) == 0) {

            wp->logging = atoi (&(argv [i][5]));

        } else if (strncmp (argv [i], "-threads=", 9) == 0) {

            wp->worker_threads = atoi (&(argv [i][9]));

            if (wp->worker_threads > MaxThreads)
                wp->worker_threads = MaxThreads;
            if (wp->worker_threads < 0)
                wp->worker_threads = 0;
        };

        i++;
    };

    // Save initial settings...

    old_workers = wp->worker_threads;
    old_port = wp->open_port;

    // Ready the Object Allocation Library

    start_objs ();

    // Dynamic loaded libraries may want these:
    _jcc_adref ("vb_popjmp", &vb_popjmp);
    _jcc_adref ("vb_pushjmp", &vb_pushjmp);
    _jcc_adref ("vb_setjmp", &vb_setjmp);
    _jcc_adref ("c_Wrap", &c_Wrap);
    _jcc_adref ("vb_error", &vb_error);

    _jcc_adref ("running", &running);
    _jcc_adref ("wp", &wp);

    _jcc_adref ("String", &String);
    _jcc_adref ("NewObject", &NewObject);

    _jcc_adref ("URLDecode", &URLDecode);
    _jcc_adref ("URLDecodeE", &URLDecodeE);
    _jcc_adref ("URLEncodeE", &URLEncodeE);
    _jcc_adref ("HTMLEncodeE", &HTMLEncodeE);

    _jcc_adref ("Application_init", &Application_init);
    _jcc_adref ("Server_init", &Server_init);

    _jcc_adref ("Session_init", &Session_init);
    _jcc_adref ("Request_init", &Request_init);
    _jcc_adref ("Response_init", &Response_init);
    _jcc_adref ("ObjectContext_init", &ObjectContext_init);

    _jcc_adref ("Collection_init", &Collection_init);

    _jcc_adref ("VBError_init", &VBError_init);

    _jcc_adref ("Variant_init", &Variant_init);
    _jcc_adref ("String_init", &String_init);

    _jcc_adref ("tmp3_jcc_dd", &tmp3_jcc_dd);
    _jcc_adref ("reload", &reload);
    _jcc_adref ("enter_reload", &enter_reload);
    _jcc_adref ("exit_reload", &exit_reload);

    _jcc_adref ("add_Script", &add_Script);
    _jcc_adref ("del_Script", &del_Script);
    _jcc_adref ("zap_Script", &zap_Script);

    _jcc_adref ("__ascii_to_ebcdic", &__ascii_to_ebcdic);
    _jcc_adref ("__ebcdic_to_ascii", &__ebcdic_to_ascii);

    _jcc_adref ("shead", &shead);

    _jcc_adref ("server_index", &server_index);

    /***/
    /***/
    /*** Start Load Dynamic Objects ***/

    // 'error' is called for any page which can't be found also!
    if ((error = reload (NULL, "/", "error", "asp", "Error_init", "Error_exit", "Error_run")) == NULL) {

        printf ("There was an error compiling/loading the 'error' page:\n");

        // Normally: output goes back to the browser, but this time - go to stdout - there's no browser.
        fh = fopen (tmp3_jcc_dd, "r"); // Open the STDOUT file from the compiler...
        if (fh) {
            while (fgets (line, 81, fh))
                printf ("%s", line);
            fclose (fh);
        };

        exit_reload ();
        goto no_dyn;
    };

    /*** End Load Dynamic Objects ***/
    /***/
    /***/

    // Delayed loading of a script... for the admin project
    //
    // this also shows how 'admin' would add user-pages to the 'system'...
#ifndef admin_dyn
    add_Script2 ("/", "admin", "asp", "Admin_init", "Admin_exit", "Admin_run",
                 (void *)_jcc_object (&Admin_init, &Admin_exit), &Admin_init, &Admin_run, 0x00000100);
#else
    if ((shead != NULL) && (shead->next == NULL)) // ie. load failed
        add_Script2 ("/", "admin", "asp", "Admin_init", "Admin_exit", "Admin_run", NULL, NULL, NULL, 0);
#endif

    /***/

    sp = (SERVER *)_new_object (Server_init);
    if (sp == NULL) {printf ("Couldn't create a Server.\n");r = 12; goto no_server;};

    _push_object (sp); // There's only one Server Object, make it so.
    // This is important, since the new threads will also use this pointer.

    ap = (APPLICA *)_new_object (Application_init);
    if (ap == NULL) {printf ("Couldn't create an Application.\n"); r= 12; goto no_appl;};

    rp = _new_object (Request_init);
    if (rp == NULL) {printf ("Couldn't create a Request.\n"); r = 12; goto no_others;};
    pp = _new_object (Response_init);
    if (pp == NULL) {printf ("Couldn't create a Response.\n"); r = 12; goto no_response;};
    op = _new_object (ObjectContext_init);
    if (op == NULL) {printf ("Couldn't create an ObjectContext.\n"); r = 12; goto no_objcont;};

    //
    // At this point, many users can be created...
    // each one able to process a single transaction
    // in a single thread for a single program-script
    // but for any requesting session (true user.)

    default_used = 0;
    upd = (void *)_new_object ((scriptd = get_Script ("/", "error", "asp"))->obj);
    if (upd == NULL) {printf ("Couldn't create a User-Process.\n"); r = 12; goto no_userp;};

    np = _new_object (Session_init);
    if (np == NULL) {printf ("Couldn't create a Session.\n"); r = 12; goto no_sessn;};

    /* Got everything needed for a minimal system... */

    rectify_threads (wp); // Start as many threads as needed - 0 is accepted: runs in this thread.
    //if (started_threads == 0) {printf ("Worker threads failed to start.\n"); r = 12; goto no_threads;};

    serv_soc = socket (PF_INET, SOCK_STREAM, 0);
    if (serv_soc == -1) {printf ("Couldn't create a Socket.\n"); r = 12; goto no_socks;};
    
    if (strcmp (wp->open_card, "any") != 0) {
        inet_aton (wp->open_card, (void *)&serv_ip);
    } else {
        serv_ip = htonl (INADDR_ANY);
    };

    S_adx.sin_family = AF_INET;
    S_adx.sin_addr.s_addr = serv_ip;
    S_adx.sin_port = htons (wp->open_port); // The port to open...

    if (bind (serv_soc, (LPSOCKADDR)&S_adx, sizeof (S_adx)) == -1) {
        printf ("Couldn't bind the Server Socket to NetWork '%s:%d'.\n", wp->open_card, wp->open_port);
        printf ("Is a Web Server already running at that address?\n");
        closesocket (serv_soc);
        goto no_socks;
    };

    if (listen (serv_soc, SOMAXCONN) == -1) {
        printf ("Couldn't listen to/on the Socket.\n");
        closesocket (serv_soc);
        goto no_socks;
    };

    i = 0;
    while (1) {

        // Get a SOCKET...
        soc = accept (serv_soc, &sa, &sz_sa);
        if (soc == -1) {
            
            // Expect this when Hercules has all it's sockets reset!
            if (errno == ENOTSOCK) {
                printf ("Sockets subsystem was shutdown...  Terminating Server...\n");
                goto shut_down;
            };

            if (++i == 100) {
                printf ("Too many errors in a row...  Terminating Server...\n");
                goto shut_down;
            };

            printf ("Error %d 'accept'ing from the master listener socket.\n", errno);
            continue;
        };
        i = 0;

        // Assign SOCKET to Request &
        rp->soc = soc;
        rp->client_ip = sap->sin_addr.s_addr; // is the client-browser or proxy.

        // Assign SOCKET to Response (where it will eventually be closed.)
        pp->soc = soc;

        // Run the Request Socker-Read-Initialiser
        rp->Start ();

        // Set identical HTTP/1.0 or HTTP/1.1 headers.
        pp->http_version = rp->http_version;

        // Sort out Cookie (if one was found that can be used.)
        if ((np2 = Get_Cookie (rp)) == NULL) {

            // Mark default as used & to be refilled.
            np2 = np;
            np = NULL; // Create a new one, we used this one.

            // Assign Set-Cookie to Response Object
            pp->Session = np2;
            pp->send_cookie = 1;

            // Monitor memory:
            // Check that memory resources are free (ask admin what size region is to be assumed.)
            // - Reject new Session requests when the system is considered in a 'critical' state.
            if ((wp->sessions_max) && (cpcnt >= wp->sessions_max)) {

                strcpy (rp->Msg, "HTTP/1.1 503 Service Unavailable");
                pp->send_code = 503;
                goto no_userp_error;
            };
        };

        // Check if it's time to scan the directory again?
        this_time = time (NULL);
        rp->start_time = this_time;

        if ((this_time - last_time) > 2) { // Check every 3 secs!
            scan_directory ();
            last_time = j;

            if ((wp->logging) && (wp->logopts))
                FlushLog (); // Flush the log stack also...
        };

        // Check if request.Path|File are new pages that required reloading...
        script = get_Script (rp->Path, rp->File, rp->Ext);
        if ((script) &&
            ((script->ttr == 0) ||        // File-Updated...
             (script->dynobp == NULL))) { // Delayed loading...

            j = wp->worker_threads;
            if (j)
                flush_threads (wp);

            enter_reload (); // Lock the STDOUT file from the compiler...
            
            script = get_Script (rp->Path, rp->File, rp->Ext); // Again, I just got the LOCK...
            if ((script) &&
                ((script->ttr == 0) ||        // File-Updated...
                 (script->dynobp == NULL))) { // Delayed loading...

                if ((strcmp (rp->Path, "/") == 0) &&
                    (strcmp (rp->File, "error") == 0) &&
                    (strcmp (rp->Ext, "asp") == 0)) {

                    _del_object (upd);

                    o = script->dynobp;
                    if (reload (o, rp->Path, rp->File, rp->Ext, script->init, script->exit, script->arun) == o) {

                        // failed to reload... don't exit_reload!

                        upd = (void *)_new_object (scriptd->obj); // Using the old scriptd!

                        pp->send_code = 0; // Compile Error!
                        goto no_userp_error;
                    };

                    scriptd = get_Script ("/", "error", "asp");
                    upd = (void *)_new_object (scriptd->obj);

                } else {
                    o = script->dynobp;
                    if (reload (o, rp->Path, rp->File, rp->Ext, script->init, script->exit, script->arun) == o) {

                        // failed to reload... don't exit_reload!

                        pp->send_code = 0; // Compile Error!
                        goto no_userp_error;
                    };
                };

                script = get_Script (rp->Path, rp->File, rp->Ext); // Again!
            };

            exit_reload ();
        };

        // if page-request isn't a 'special' one...
        if (script != NULL) {

            up = (void *)_new_object (script->obj);
            if (up == NULL) goto no_userp_error;
            launch_next (script->run, up, sp, ap, np2, rp, pp, op);

        } else {

            if (rp->Msg [0] == 0) {
                strcpy (rp->Msg, "HTTP/1.1 404 Not Found");
                pp->send_code = 404;
            };
no_userp_error:
            launch_next (scriptd->run, upd, sp, ap, np2, rp, pp, op);
            default_used = 1;
        };

        if (running == 0) { // Asked to quit...?
shut_down:
            if (wp->save_disable == 0)
                save (ini_file, wp);
            
            wp->worker_threads = 0;
            rectify_threads (wp); // Stop threads
            
            if ((wp->logging) && (wp->logopts))
                FlushLog (); // Flush the log stack also...

            goto stop_sockets; // Quitting...
        };

        if (wp->save_trigger) {
            wp->save_trigger = 0;

            if (wp->save_disable == 0)
                save (ini_file, wp);
        };

        if (old_workers != wp->worker_threads) {

            if (wp->worker_threads > MaxThreads)
                wp->worker_threads = MaxThreads;
            if (wp->worker_threads < 0)
                wp->worker_threads = 0;

            old_workers = wp->worker_threads;
            rectify_threads (wp); // Change threads
        };

        if (wp->open_port != old_port) {
            old_port = wp->open_port;

            closesocket (serv_soc);
            serv_soc = socket (PF_INET, SOCK_STREAM, 0);
            S_adx.sin_port = htons (wp->open_port); // The new port to open...
            bind (serv_soc, (LPSOCKADDR)&S_adx, sizeof (S_adx));
            listen (serv_soc, SOMAXCONN);
        };

        rp = _new_object (Request_init);  // Get ready for a new incomming connection...
        pp = _new_object (Response_init);
        op = _new_object (ObjectContext_init);

        if (np == NULL)
            np = _new_object (Session_init);

        if (default_used) {
            upd = (void *)_new_object (scriptd->obj);
            default_used = 0;
        };
    };

stop_sockets:
    closesocket (serv_soc);
    goto no_others; // let any existing objects sort themselves out

no_threads:

no_socks:
    _del_object (np);
no_sessn:
    _del_object (upd);
no_userp:
    _del_object (op);

no_objcont:
    _del_object (pp);
no_response:
    _del_object (rp);

no_others:
    _del_object (ap);
no_appl:
    _pop_object (sp);
    _del_object (sp);

no_server:
    unload_Scripts ();

no_dyn:
    Kill_Cookie (); // Remove the Cookie/Session Objects stored here.
    stop_objs ();

    wdel (wp);

    if (loghandle)
        fclose (loghandle);

    printf ("Finished: %d max. bytes used, %d bytes leaked.  %d max. stack used.\n",
        __libc_heap_max, __libc_heap_used - initmem, __libc_stack_max);

    return (r);
};
