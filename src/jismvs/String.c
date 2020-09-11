/*
 * JIS MVS3.8j WWW Application Server.  (C)2004 Jason Paul Winter, All Rights Reserved.
 *
 * jasonwinter@hotmail.com, use "Subject" wisely - spam gets deleted without being read.
 *
 */

#define String 1
#include "asp.h"
#undef String

// String object.
//
// Strings and all string operations...

int String_init ();

// Properties

static long   Length = 0;   // A shortcut to the end of string.
static long   Size   = 0;   // The maximum bytes (incl. nul) the string can be.
static char * Value = NULL; // A C Pointer to the nul-terminated string.

// Methods

static char * (*String) ();

// Code

static void * Copy (void * o) {

    return (NULL);
};

static void * NS_push_use (char * s) {

    Length = strlen (s);
    Size = Length + 1;

//    Size = Length + (0x100 - (Length & 0xFF)); // Round up to 256 chars

    Value = (char *)malloc (Size);

    if (Value)
        memcpy (Value, s, Size);

    return (Value);
};

void * NewString (char * s) {
    void * o;

    if (s == NULL) return (NULL);

    o = _new_object (&String_init);
    if (o == NULL) return (NULL);

    _push_object (o);

    if (NS_push_use (s) == NULL) { // Our 80(R13) hasn't been updated...

        _pop_object (o);
        _del_object (o);
        return (NULL);
    };

    _pop_object (o);

    return (o);
};

static char * c_String (int size, int val) {
    void * a;

    a = (void *)malloc (size);
    memset (a, val, size);

    return (a);
};

static char * InStr (int start, char * s1, char * s2, int cmp) {

    return (NULL);
};

int String_init () {

#ifdef DEBUG
    printf ("String Created.\n");
#endif

    return (1);
};

int String_exit () {

    if (Value)
        free (Value); // Simplistic!

#ifdef DEBUG
    printf ("String Deleted.\n");
#endif

    return (1);
};
