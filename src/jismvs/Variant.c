/*
 * JIS MVS3.8j WWW Application Server.  (C)2004 Jason Paul Winter, All Rights Reserved.
 *
 * jasonwinter@hotmail.com, use "Subject" wisely - spam gets deleted without being read.
 *
 */

#include <process.h>

#define Variant
#include "asp.h"

extern void(*__libc_ECS)(LPCRITICAL_SECTION lock);
extern void(*__libc_LCS)(LPCRITICAL_SECTION lock);

// Variant object.
//
// Type 'OBJECT' is special, since Sessions may expire & be freed
// this code must call the destructors to prevent memory leaks.
//
// The instance_count ensures you may duplicate an object pointer
// with the 'Set' command - and prevent the deletion until both are freed.

int Variant_init ();

// Properties

static long instance_count = 0;

static enum Type ValueType;
static int  SubType = 0;
static union Values  Value;

// Methods

static void (*instance_count_inc) ();

// Globals

static CRITICAL_SECTION lock;

// Code

static void c_ici (int offset) {
    if (__libc_ECS) (*__libc_ECS) (&lock);
    instance_count += offset;
    if (__libc_LCS) (*__libc_LCS) (&lock);
};

static void * S_push_use (char * s) {

    ValueType = Object; // String is an Object - pay the price of two...
    SubType = 1; // String
    Value.ov = NewString (s);

    return (Value.ov);
};

void * String (char * s) { // A Real String (can't be defined in String!!)...
    void * a;

    a = _new_object (&Variant_init);
    if (a == NULL) return (NULL);

    _push_object (a);

    if (S_push_use (s) == NULL) {

        _pop_object (a);
        _del_object (a);
        return (NULL);
    };

    _pop_object (a);

    return (a);
};

static void * C_push_use (void * o) {

    ValueType = Object;
    SubType = 3; // Generic Object for now...
    
    Value.ov = _new_object (o);

    return (Value.ov);
};

void * NewObject (void * o) { // A Real Collection (SafeArray)... User-Object.
    void * a;

    a = _new_object (&Variant_init);
    if (a == NULL) return (NULL);

    _push_object (a);

    if (C_push_use (o) == NULL) {

        _pop_object (a);
        _del_object (a);
        return (NULL);
    };

    _pop_object (a);

    return (a);
};

int Variant_init (void * me) {

    instance_count_inc = _make_f_vector (&c_Wrap, &c_ici, me);

    ValueType = None;
    instance_count++;

#ifdef DEBUG
    printf ("Variant Created.\n");
#endif

    return (1);
};

int Variant_exit () {

    if (__libc_ECS) (*__libc_ECS) (&lock);

    if (--instance_count <= 0) {
        
        if (ValueType == Object)
            _del_object (Value.ov);
        
        _del_f_vector (instance_count_inc);

        if (__libc_LCS) (*__libc_LCS) (&lock);

        return (1); // Ack. deletion
    };

    if (__libc_LCS) (*__libc_LCS) (&lock);

#ifdef DEBUG
    printf ("Variant Deleted.\n");
#endif

    return (0); // Prevent deletion
};
