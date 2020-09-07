/*
 * JIS MVS3.8j WWW Application Server.  (C)2004 Jason Paul Winter, All Rights Reserved.
 *
 * jasonwinter@hotmail.com, use "Subject" wisely - spam gets deleted without being read.
 *
 */

#include <time.h>

#define Session 1
#include "asp.h"

// Session object.
//
// A User will keep this object when the HTTP Response
// returns a 200 OK (a signal to store this instance
// into the global session-list.)

static void c_Abandon ();

// Properties

static time_t session_used_time; // Last-Used-Time.

static int Timeout = 0;          // Not used.
static int SessionID = 0;        // Not used.
static int LCID = 0;             // Not used.
static int CodePage = 0;         // Not used.

// Collections

static COLLECT * Contents;
static COLLECT * StaticObjects;

// Methods

void (*Abandon) = &c_Abandon;

// Code

static void c_Abandon () {

    session_used_time = 0; // Very old... Will be deleted.
};

int Session_init () {

    session_used_time = time (NULL);

    // Create: Contents
    Contents = _new_object (Collection_init);

    // Create: StaticObjects
    StaticObjects = _new_object (Collection_init);

#ifdef DEBUG
    printf ("Session Created.\n");
#endif

    return (1);
};

int Session_exit () {

    // Delete: StaticObjects
    _del_object (StaticObjects);

    // Delete: Contents
    _del_object (Contents);

#ifdef DEBUG
    printf ("Session Deleted.\n");
#endif

    return (1);
};
