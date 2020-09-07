#include <stdio.h>
#include <jclass.h> // Access to 'object' functions

//#define first_build 1
//
// First compiled with -class=jtest.h to make Class Structure Map.
// This is complicated by the main test-harness being in this object!
// Changes to main can effect the map, which in turn effects main!
// - This makes this example difficult to alter and recompile.
// - Use generated class header files only in "other" JCC source-files.
//

// Utility: c_wrapper
// Assume caller has (f,me,cnt) added to the start of the parameters.
// The JClass library can make such vectors using _make_f_vector
void * c_wrapper (void (*f)(), void * me) {
    void * a;
    void * r;

    a = _get_vector_r ();

    _push_object (me); // Auto-use the object for which the vector is owned.

    r = _exec_vector (f, a); // Another JClass library function

    _pop_object (me); // Mark as Unused again

    return (r);
};

/*-=-=-=-=-*/ /*** class part - one class per source-file/object ***/

#include "jtest.h" // Pre-Built (before main was 'expanded'...)

// These global variables will be object-instance-specific

static int v = 1;     // Object-Global variable
static int * vp = &v; // Pointer to *REAL* Global though! &v isn't this v
// ie. new objects will be created with v=x if *vp=x is executed anywhere.

static void (*Lock) ();
static void (*Unlock) (); // Auto-use methods...  Set up with _make_f_vector.

// Function code:

static void c_Lock () {

    printf ("Locked %d.\n", v);
};

static void c_Unlock () {

    printf ("Unlocked %d.\n", v);
};

// All class-objects must have an init: (any name!)
int c_init (void * me) { // Init function (one per class file/object only.)

    v = 2; // The objects v is 2, the global v is 1 still

    Lock = _make_f_vector (&c_wrapper, &c_Lock, me); // JClass function creation.
    Unlock = _make_f_vector (&c_wrapper, &c_Unlock, me); // 0-no extra parameters

    return (1);
};

int c_term (void * me) { // 'clean-up' here (can be passed as NULL below...)

    _del_f_vector (Lock);
    _del_f_vector (Unlock);

    printf ("An object instance was deleted (v=%d.)\n", v);

    return (1);
};

void c_print () { // Data reference function test (simple method.)

    printf ("The value v=%d.\n", v);
    printf ("The global value v=%d.\n", *vp); // *vp gets the Global v
    printf ("\n");
};

/*-=-=-=-=-*/ /*** test-harness part - doesn't interact with the class data ***/

#define withobj(o, stmt) {_push_object (o); stmt; _pop_object (o);}

int main () { // must not use any globals for the test-start to be here *

// i: Use "void *" before jtest.h exists - only-if harness-in-object.

#ifdef first_build
    void * i; // One instance of an object *pointer* please...
#else
    JTEST * i;
#endif

    if (_jcc_object (c_init, c_term)) { // Register the use of class-objs

        // worked...

        i = _new_object (c_init);     // Create instance of registered obj
        if (i) { // Also worked...

            // hack into object to find the new data-size:
            printf ("Size of new instance is %d bytes.\n\n", (*((long **)i)) [4] + 12); // Calculated size of object

            //
            // Variables in the class can be accessed using normal
            // C dereferencing: i->v (exists due to v being a global.)
            // - Option to rename non-externals to _jcc_space001???
            //

            //
            // Function calls are slightly more complicated:
            // Since this object is an everyday JCC rent object...
            // calling a function "doesn't notice" any difference with
            // or without objects being present.
            //

            c_print (); // Use the default initialisation area for this!
            
            //
            // But... By adding the object to ths in-use stack, the
            // called function now only 'sees' the object variables
            // as it's globals.
            //

            _push_object (i);     // Enter the 'use' of 'new' obj
            c_print ();
            _pop_object (i);      // Exit the 'use' of it

            //
            // A macro to simplify use: (use ; in stmt if needed)
            //

            withobj (i, c_print ());

            //
            // External example (although this is internally used here)
            // of accessing object variables:
            //

// After compiling this source once: jtest.h will(1) exist for the next
// line to compile as well(2).
// 1:when / 2:if option -class=jtest.h 1:is / 2:isn't used.)

#ifndef first_build
            printf ("obj i->v=%d\n", i->v);
            printf ("obj *(i->vp)=%d\n", *(i->vp));
#endif

            //
            // Internal example (although main wouldn't normally be
            // defined as a class method) of switching contexts while
            // code is executing in another context.
            // (Local-Temp variables would be needed to copy globals!)
            //

            // ...Only because 'main' is part of the object...:
            _push_object (i);     // Enter the 'use' of 'new' obj

#ifndef first_build
            printf ("i obj v=%d\n", i->v);
#endif
            _pop_object (i);      // Exit the 'use' of it
            // Don't use this, unless you have defined entry points
            // in the object for *external* use only!
            // (functions not otherwise called by the objects functions.)
            // Last note: There would be performance issues pushing
            // -   objects unnecessarily or recursively!  Beware.

            //
            // Try the auto-use methods
            //
#ifndef first_build
            i->Lock ();
            i->Unlock ();
#endif

            _del_object (i);          // Deallocate resources & clean-up.
        } else {
        
            printf ("Unable to create a new object-instance.\n");
        };

        _end_object (c_init);         // Deallocate *all* obj-resources left.
    } else {

        printf ("Can't obtain object separation lock.\n");
    };

    return (0);
};
