/*
 * JIS MVS3.8j WWW Application Server.  (C)2004 Jason Paul Winter, All Rights Reserved.
 *
 * jasonwinter@hotmail.com, use "Subject" wisely - spam gets deleted without being read.
 *
 */

#include <process.h>

extern void(*__libc_ECS)(LPCRITICAL_SECTION lock);
extern void(*__libc_LCS)(LPCRITICAL_SECTION lock);

#include "asp.h"

// Application object.

// Properties

static COLLECT * Contents;
static COLLECT * StaticObjects;

// Methods

static void (*Lock) ();
static void (*Unlock) ();

// Globals

static CRITICAL_SECTION lock = 0;

// Code

static void c_Lock () {

    if (__libc_ECS) (*__libc_ECS) (&lock);
//    EnterCriticalSection (&lock);

#ifdef DEBUG
    printf ("Enter Locked Application.Code Section.\n");
#endif
};

static void c_Unlock () {

    if (__libc_LCS) (*__libc_LCS) (&lock);
//    LeaveCriticalSection (&lock);

#ifdef DEBUG
    printf ("Leave Locked Application.Code Section.\n");
#endif
};

int Application_init (void * me) {

    // Set up object methods
    Lock = _make_f_vector (&c_Wrap, &c_Lock, me);
    Unlock = _make_f_vector (&c_Wrap, &c_Unlock, me);

    // Create: Contents
    Contents = _new_object (Collection_init);

    // Create: StaticObjects
    StaticObjects = _new_object (Collection_init);

#ifdef DEBUG
    printf ("Application Created.\n");
#endif

    return (1);
};

int Application_exit () {

    // Delete: function vectors
    _del_f_vector (Lock);
    _del_f_vector (Unlock);

    // Delete: StaticObjects
    _del_object (StaticObjects);

    // Delete: Contents
    _del_object (Contents);

#ifdef DEBUG
    printf ("Application Deleted.\n");
#endif

    return (1);
};
