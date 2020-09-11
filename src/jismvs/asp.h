/*
 * JIS MVS3.8j WWW Application Server.  (C)2004 Jason Paul Winter, All Rights Reserved.
 *
 * jasonwinter@hotmail.com, use "Subject" wisely - spam gets deleted without being read.
 *
 */

#include <setjmp.h>
#include <jclass.h>

int URLDecode   (char * buffer, int length);
int URLDecodeE  (char * buffer, int length, char * __ascii_to_ebcdic);
int URLEncodeE  (char * buffer, int length, char * __ebcdic_to_ascii);
int HTMLEncodeE (char * buffer, int length);

//#define DEBUG 1

void _xlate (void * table, char * buf, long len);

extern unsigned char __ascii_to_ebcdic [];
extern unsigned char __ebcdic_to_ascii [];

#define atoe(b,l) _xlate (__ascii_to_ebcdic, b, l)
#define etoa(b,l) _xlate (__ebcdic_to_ascii, b, l)

#ifndef NULL
#define NULL 0
#endif

void   vb_pushjmp (void * aux); // Enter new error arrangement
void   vb_setjmp ();            // Mark this stack-level with the last pushjmp
void * vb_popjmp ();            // pop to old error arrangement
                                // - markers allow normal 'runtime-ends' to be captured.
#define On_Error_Resume_Next {vb_pushjmp (NULL); ErrorInstance->on_error = 2;}
#define On_Error_Goto(l) {vb_pushjmp (NULL); ErrorInstance->on_error = 1; if (setjmp (*(ErrorInstance->jmp_error))) goto l;}
#define On_Error_Goto_0 {vb_pushjmp (NULL); ErrorInstance->on_error = 0;}

// Must use these exit macros, if On_Error was used:
#define Exit_Function(v) {vb_popjmp (); return (v);}
#define Exit_Sub {vb_popjmp (); return;}

void * c_Wrap (void (*f)(), void * me);

void vb_error (char * object, char * function, char * msg, ...);

// Variants

enum Type {
    None,
    Char,
    Short,
    Long,
    LongLong,
    Float,
    Double,
    Object
};

union Values { // The storage (enough for every type needed.)
    char cv;
    short sv;
    long lv;
    long long llv;
    float fv;
    double dv;
    void * ov; // Object (User, built-in or String (or another Variant))
};

// Strings:

void * NewString (char * s); // Used in String just-below...

// Conditionally include, depending on object-use requirements

#ifndef String

void * String (char * s); // A Real String (can't be defined in String!!)...

void * NewObject (void * o); // A Save-Array (Collection/User-Obj within a Variant.)

 #include "String.h"

#ifndef Variant

 #include "Variant.h"

// Collections (list - for For/Next programming.)

typedef struct list_tag * list_ptr;
typedef struct list_tag {
    VARIANT  * item;
    char     * key;
    int        position;
    list_ptr   next;
} list_sz;

#ifndef Collection

 #include "Collection.h"

#ifndef ObjectContext

 #include "ObjectContext.h"

#ifndef Request

 #include "Request.h"

#ifndef Session

 #include "Session.h"

// Cookie-Processing for Sessions (performed in Response...)
char * New_Cookie (SESSION * s); // Returns a pointer to a string with a new cookie.

#ifndef Response

 #include "Response.h"

#ifndef vberror

 #include "VBError.h"

#ifndef Server

 #include "Application.h"
 #include "Server.h"

#endif // Server
#endif // vberror
#endif // Response
#endif // Session
#endif // Request
#endif // ObjectContext
#endif // Collection
#endif // Variant
#endif // String

int Application_init ();
int Application_exit ();
int Collection_init ();
int Collection_exit ();
int ObjectContext_init ();
int ObjectContext_exit ();
int Response_init ();
int Response_exit ();
int Request_init ();
int Request_exit ();
int Session_init ();
int Session_exit ();
int Server_init ();
int Server_exit ();
int Variant_init ();
int Variant_exit ();
int VBError_init ();
int VBError_exit ();
int String_init ();
int String_exit ();

// ServerVariables
//
// _'s are -'s in the RAW data and also processed to UPPERCASED identifiers
/*
    ALL_HTTP,      // Similar to RAW but with "HTTP_" added to 'others'
    ALL_RAW,       // All as-sent
    AUTH_PASSWORD, // Basic auth. only
    AUTH_TYPE,     // (method used)
    AUTH_USER,     // RAW Username
    CONTENT_LENGTH,// The form-data
    CONTENT_TYPE,  // GET/POST/PUT
    LOCAL_ADDR,    // Adaptor in use
    PATH_INFO,     // Extra. Info.
    PATH_TRANSLATED,// MVS version of path!
    QUERY_STRING,  // following '?'!
    REMOTE_ADDR,   // remote host
    REMOTE_HOST,   // remote hostname
    REMOTE_USER,   // users typed in name!
    REQUEST_METHOD,// GET/HEAD/POST
    SCRIPT_NAME,   // Virtual path of script
    SERVER_NAME,   // How to generate self-ref URLs
    SERVER_PORT,   // the port
    URL,           // the base part of the URL
    SERVER_PROTOCOL,// "HTTP/1.1"
    SERVER_SOFTWARE// name/version
*/
