/*
 * JIS MVS3.8j WWW Application Server.  (C)2004 Jason Paul Winter, All Rights Reserved.
 *
 * jasonwinter@hotmail.com, use "Subject" wisely - spam gets deleted without being read.
 *
 */

#define Server 1
#include "asp.h"

// Server object.

// Properties

static int ScriptTimeout = 0; // Not used.

// Methods

void (*HTMLEncode) ();
void (*URLEncode) ();

// Code

static void c_HTMLEncode (VARIANT * v) {
    long   i;
    char * a;

    // Resize memory in STRING to 2 times Length (not Size.)
    if (((STRING *)(v->Value.ov))->Length * 2 >= ((STRING *)(v->Value.ov))->Size) {

        i = (((STRING *)(v->Value.ov))->Length * 2) + 1;
        a = (char *)malloc (i);
        memcpy (a, ((STRING *)(v->Value.ov))->Value, ((STRING *)(v->Value.ov))->Length + 1);
        free (((STRING *)(v->Value.ov))->Value);
        ((STRING *)(v->Value.ov))->Value = a;
        ((STRING *)(v->Value.ov))->Size = i;
    };

    // Process it.
    ((STRING *)(v->Value.ov))->Length = HTMLEncodeE (((STRING *)(v->Value.ov))->Value, ((STRING *)(v->Value.ov))->Length);
};

static void c_URLEncode (VARIANT * v) {
    long   i;
    char * a;

    // Resize memory in STRING to 3 times Length (not Size.)
    if (((STRING *)(v->Value.ov))->Length * 3 >= ((STRING *)(v->Value.ov))->Size) {

        i = (((STRING *)(v->Value.ov))->Length * 3) + 1;
        a = (char *)malloc (i);
        memcpy (a, ((STRING *)(v->Value.ov))->Value, ((STRING *)(v->Value.ov))->Length + 1);
        free (((STRING *)(v->Value.ov))->Value);
        ((STRING *)(v->Value.ov))->Value = a;
        ((STRING *)(v->Value.ov))->Size = i;
    };

    // Process it.
    ((STRING *)(v->Value.ov))->Length = URLEncodeE (((STRING *)(v->Value.ov))->Value, ((STRING *)(v->Value.ov))->Length, __ebcdic_to_ascii);
};

int Server_init (void * me) {

    HTMLEncode = _make_f_vector (&c_Wrap, &c_HTMLEncode, me);
    URLEncode = _make_f_vector (&c_Wrap, &c_URLEncode, me);

#ifdef DEBUG
    printf ("Server Created.\n");
#endif

    return (1);
};

int Server_exit () {

    _del_f_vector (URLEncode);
    _del_f_vector (HTMLEncode);

#ifdef DEBUG
    printf ("Server Deleted.\n");
#endif

    return (1);
};
