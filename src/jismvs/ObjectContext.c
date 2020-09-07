/*
 * JIS MVS3.8j WWW Application Server.  (C)2004 Jason Paul Winter, All Rights Reserved.
 *
 * jasonwinter@hotmail.com, use "Subject" wisely - spam gets deleted without being read.
 *
 */

#define ObjectContext 1
#include "asp.h"

// ObjectContext object.

static void c_SetComplete ();
static void c_SetAbort ();

// MTS transaction coordinator

// Methods

void (* SetComplete) () = &c_SetComplete;
void (* SetAbort) () = &c_SetAbort;

// Code

static void c_SetComplete () {
};

static void c_SetAbort () {
};

int ObjectContext_init () {

#ifdef DEBUG
    printf ("ObjectContext Created.\n");
#endif

    return (1);
};

int ObjectContext_exit () {

#ifdef DEBUG
    printf ("ObjectContext Deleted.\n");
#endif

    return (1);
};
