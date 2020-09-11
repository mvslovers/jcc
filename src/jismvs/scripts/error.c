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

extern char tmp3_jcc_dd []; // Compiler messages-file (STDOUT)
extern int __libc_heap_max;
extern int __libc_stack_max;

// User object.
//
// Anything - but in script format (This is a template script:)
//
// Template: basically output an echo - everything that was sent by the browser.
//

// *** Normally a Script doesn't need these values: ***
extern long running;   // We may request the server to finish
extern WebSite_ptr wp; // We may request single-instance mode
// ***

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

int Error_init (void * me) { // Could store void* 'me' for later?  Optional.

#ifdef DEBUG
    printf ("User Created.\n");
#endif

    return (1); // OK to Create!
};

int Error_exit () { // could also define void * me as a parameter here... Optional.

#ifdef DEBUG
    printf ("User Deleted.\n");
#endif

    return (1); // OK to Deallocate!
};

#ifdef DEBUG

static void write_basic (VARIANT * v) {

    switch (v->ValueType) {
    case Char:
        if ((__ebcdic_to_ascii [v->Value.cv] >= 0x20) &&
            (__ebcdic_to_ascii [v->Value.cv] < 0x7F))
            Response->Write ("'%c'<br>\n", v->Value.cv);
        else
            Response->Write ("%d<br>\n", v->Value.cv);
        break;
    case Short:
        Response->Write ("%d<br>\n", v->Value.sv);
        break;
    case Long:
        Response->Write ("%d<br>\n", v->Value.lv);
        break;
    case LongLong:
        Response->Write ("%lld<br>\n", v->Value.llv);
        break;
    case Float:
        Response->Write ("%.17G<br>\n", ((double)v->Value.fv));
        break;
    case Double:
        Response->Write ("%.17G<br>\n", v->Value.dv);
        break;
    case None:
    default:
        Response->Write ("&lt;Empty&gt;<br>\n");
        break;
    };
};

//
// walker: prints to Response - each item within a Collection (up to 2 dims.)
//
static void walker (COLLECT * c) {
    long      i;
    list_ptr  f;
    list_ptr  f2;
    VARIANT * v;
    VARIANT * v2;

    //
    // All Collection Variants are STRING*'s or COLLECT*'s... (safe-arrays)
    //

    v = c->For (&f);
    while (v) {

        // See note about SubType below in ASP_run...
        // I hope 'walker' isn't used on User-Objects still marked as SubType == 2!

        if (v->SubType == 2) { // Same as HasKeys = TRUE (need to use ItemIndexed (key, index))

            // Is a nested Collection (Safe-Array or Keyed Cookie Library.)
            // - we support a second dimension... here goes...
            // - "of course" an ABEND will occur if SubType==2 and v isn't a Collection after all.
            
            i = 1;
            v2 = ((COLLECT *)(v->Value.ov))->For (&f2);
            while (v2) {

                if (v2->SubType < 2) { // Don't process 2-up...

                    if (f2->key)
                        Response->Write ("%s(%s) = ", f->key, f2->key);
                    else
                        Response->Write ("%s(%d) = ", f->key, i);

                    if (v2->SubType) // Standard String...
                        Response->Write ("%s<br>\n", ((STRING *)(v2->Value.ov))->Value);
                    else
                        write_basic (v2);
                };

                v2 = ((COLLECT *)(v->Value.ov))->Next (&f2);
                i++;
            };

        } else if (v->SubType == 1) { // Don't care if it's a SubType == 0 (basic data type)
        
            Response->Write ("%s = %s<br>\n", f->key, ((STRING *)(v->Value.ov))->Value);

        } else if (v->SubType == 0) { // A user-Object with SubType == 0 will just print the last basic value

            Response->Write ("%s = ", f->key);
            write_basic (v);

        } else {
            Response->Write ("%s = &lt;Object&gt;<br>\n", f->key);
        };

        v = c->Next (&f);
    };
};

#endif // DEBUG

//
// Some REAL GLOBAL variables... (should 'lock' a critical-section around the use of these!)
//
static int assign_index = 0;      // Assign dummy application user-ids based on this number...
static int * aip = &assign_index; // Make a Global available...
//
// The 'code' (was:interpreter) this function name is 'registered' along with User_init/User_exit...
//
void Error_run (void * me, SERVER * sp, APPLICA * ap, SESSION * np, REQUEST * rp, RESPONS * pp, OBJECTC * op, VBERROR * ip) {
    VARIANT * v;
    char      b [32];
    char      line [82];
    FILE    * fh;

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

#ifdef DEBUG

    /** ** ** ** ** **/
    Request->QueryString_Start (); // We want a Collection (this is needed to perform the work of building it)
    Request->Form_Start ();        // We want a Collection (this is needed to perform the work of building it)
    /** ** ** ** ** **/

#endif

    //
    // READY!
    //

    Response->Write ("<HTML><BODY>\n"); // Set up text output to a browser document...

/* TEST .Redirect function... works. *

    if ((strcmp (Request->Path, "/") != 0) ||
        (strcmp (Request->File, "error") != 0) ||
        (strcmp (Request->Ext, "asp") != 0)) {

        Response->Redirect ("HTTP://127.0.0.1:81/error.asp");

        goto ASP_exit;
    };
**/

    if (Response->send_code == 0) { // A special signal from the script-builder...

        // Write the last compiler message-state:
        fh = fopen (tmp3_jcc_dd, "r"); // Open the STDOUT file from the compiler...
        if (fh) {

            Response->Write ("<b>There was an error compiling the page:</b>\n<p>\n");

            Response->Write ("<code><pre>");
        
            while (fgets (line, 81, fh))
                Response->Write ("%s", line);

            fclose (fh);

            Response->Write ("</pre></code>\n<p>\n");
        };

        exit_reload ();            // We picked up the lock
        Response->send_code = 200; // back to normal, give'em-a-Session!
    };


    // Sessions...  Works.  The Session String is stored and brought back.
    v = Session->Contents->Item ("UserName");

    if ((v == NULL) || (v->ValueType != Object) || (v->SubType != 1)) {
        // SubType == 0 is a low-level datatype (see asp.h for emun Type and union Values)
        // SubType == 1 is a String. (Type=Object)
        // SubType == 2 is an Array made using a new child-Collection. (Type=Object)
        // SubType >= 3 is a User-Type. (also uses Type=Object) Set by fn:"NewObject"
        //
        // User-Objects can be kept track of by changing the SubType to be >3 and
        // unique for each object type you store within an application script.
        // (ie. This should be done after calls to NewObject.)

        // Here, we know it's a String we want... and not what we've got...
        sprintf (b, "Guest_%06d", (*aip)++); // Global Variable!
        v = String (b);
        
        // AddUpdate is a simpler insert method which also first deletes any old-stored Object.
        // This is faster than a Remove/Add combination of calls.
        Session->Contents->AddUpdate (v, "UserName");

        // Write out message before potentially deleting v...
        Response->Write ("Welcome %s,<p>\n", ((STRING *)(v->Value.ov))->Value);

        // v was a temp... and we nolonger want the count value to say v holds the Object...
        _del_object (v); // Only actually deletes the Object if it wasn't added/updated properly.

    } else
        // And Here (else) we know it's a String and we got it! (or check for any User-Object SubType)
        Response->Write ("Welcome %s,<p>\n", ((STRING *)(v->Value.ov))->Value);

    Response->Write ("Max. Heap used: %d bytes (%08X)  Max. Stack used: %d bytes (%08X)<p>\n",
        __libc_heap_max, __libc_heap_max, __libc_stack_max, __libc_stack_max);

    if (Request->Path [0])
        Response->Write ("Path: %s<br>\n", Request->Path);
    if (Request->File [0])
        Response->Write ("File: %s<br>\n", Request->File);
    if (Request->Ext [0])
        Response->Write ("Ext: %s<p>\n", Request->Ext);

    if (Request->Msg [0]) {
        if (Response->send_code == 200)
            Response->send_code = 400; // Maybe don't need a Session
        Response->Write ("Error: %s<p>\n", Request->Msg);
    };

#ifdef DEBUG

    // Request.ServerVariables

    walker (Request->ServerVariables);
    Response->Write ("<br>\n");

    // Request.Cookies - SPECIAL - in that Cookie-arrays can have named elements.
    
    walker (Request->Cookies);
    Response->Write ("<br>\n");

    // Request.QueryString

    walker (Request->QueryString);
    Response->Write ("<br>\n");

    // Request.Form

    walker (Request->Form);

    Response->Write ("<FORM METHOD=POST><INPUT TYPE=\"SUBMIT\">");
    Response->Write ("<INPUT TYPE=\"HIDDEN\" NAME=\"A\" VALUE=\"CCC\">");
    Response->Write ("<INPUT TYPE=\"TEXT\" NAME=\"A\" VALUE=\"ABC\">");
    Response->Write ("<INPUT TYPE=\"HIDDEN\" NAME=\"A\" VALUE=\"BBB\"></FORM>\n");

#endif

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
