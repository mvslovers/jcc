/*
 * JIS MVS3.8j WWW Application Server.  (C)2004 Jason Paul Winter, All Rights Reserved.
 *
 * jasonwinter@hotmail.com, use "Subject" wisely - spam gets deleted without being read.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "asp.h"
#include "web.h"

// Hello World object.

// Properties (None in User-Top-Level Script Objects.)

// Methods (None in User-Top-Level Script Objects.)

// Globals (Just the passed-in values, which we make available to this entire Object:)

static REQUEST * Request;
static RESPONS * Response;

static OBJECTC * ObjectContext;

static APPLICA * Application;
static SERVER  * Server;
static SESSION * Session;

static VBERROR * ErrorInstance;

static long took_lock; // Did I lock the Application and get an error? (Good programming practice)

#define Take_Lock {Application->Lock (); took_lock = 1;}
#define Give_Lock {Application->Unlock (); took_lock = 0;}

// Code (Don't need to do very much in a Script-type Object here:)

int Hello_init (void * me) { // Could store void* 'me' for later?  Optional.

#ifdef DEBUG
    printf ("Hello Created.\n");
#endif

    return (1); // OK to Create!
};

int Hello_exit () { // could also define void * me as a parameter here... Optional.

#ifdef DEBUG
    printf ("Hello Deleted.\n");
#endif

    return (1); // OK to Deallocate!
};

void Hello_run (void * me, SERVER * sp, APPLICA * ap, SESSION * np, REQUEST * rp, RESPONS * pp, OBJECTC * op, VBERROR * ip) {

    Server = sp;        // One and only
    Application = ap;   // Global vars
    Session = np;       // Found against cookie

    Request = rp;
    Response = pp;      // These are transaction specific
    ObjectContext = op;

    ErrorInstance = ip; // This is needed to use the On_Error macros...
                        // - not recommended to go without it.

    // For the Application Object:
    took_lock = 0;      // Remember to mark any Locks without an Unlock (errors and setjmp?)

    On_Error_Goto (failed); // Initialise this stack-level jmp point.
    vb_setjmp ();           // Mark this level against the (assumed) object-push level we got from main.

    /** ** ** ** ** **/
    Request->QueryString_Start (); // We want a Collection (this is needed to perform the work of building it)
    Request->Form_Start ();        // We want a Collection (this is needed to perform the work of building it)
    /** ** ** ** ** **/

    //
    // READY!
    //

    Response->Write ("<HTML><BODY>\n"); // Set up text output to a browser document...

    Response->Write ("Hello World.\n");

ASP_exit:

    Response->Write ("</BODY></HTML>\n"); // Errors always close the HTML document in this script.

    //
    // FINISHED!
    //

    if (took_lock)
        Application->Unlock (); // Don't ever want to leave this locked!

    // Needs to be the same as Exit_Sub
    vb_popjmp (); // & Use the marker to reset everything
    return;

failed:
    Response->Write ("Error in ASP, page generation stopped.<br>\n");
    goto ASP_exit;
};
