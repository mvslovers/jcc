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

extern int __libc_heap_max;
extern int __libc_stack_max;

int add_Script (char * path, char * name, char * ext, char * init, char * exit, char * arun);
extern long server_index; // The 'Server' unique-id generated in the SESSION Cookie-Name itself.

// Admin object.
//
// Empty at the moment, but should eventually allow new scripts to
// be added to the system and allow the WebServer to be configured.

// *** Normally a Script doesn't need these values: ***
extern long running;   // We may request the server to finish
extern WebSite_ptr wp; // We may request single-instance mode
extern Scripts_ptr shead;
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

int Admin_init (void * me) { // Could store void* 'me' for later?  Optional.

#ifdef DEBUG
    printf ("Admin Created.\n");
#endif

    return (1); // OK to Create!
};

int Admin_exit () { // could also define void * me as a parameter here... Optional.

#ifdef DEBUG
    printf ("Admin Deleted.\n");
#endif

    return (1); // OK to Deallocate!
};

//  < 0 : element1 is less than element2
//  = 0 : element1 is equal to element2
//  > 0 : element1 is greater than element2
static int my_cmp1 (Scripts_ptr * a, Scripts_ptr * b) {
    int i;

    i = stricmp ((*a)->path, (*b)->path);
    if (i == 0) {
        i = stricmp ((*a)->name, (*b)->name);
        if (i == 0) {
            i = stricmp ((*a)->ext, (*b)->ext);
            if (i == 0)
                return (0);
            else
                return (i);
        } else
            return (i);
    } else
        return (i);
};

static int my_cmp2 (Scripts_ptr * a, Scripts_ptr * b) {
    int i;

    i = stricmp ((*a)->name, (*b)->name);
    if (i == 0) {
        i = stricmp ((*a)->path, (*b)->path);
        if (i == 0) {
            i = stricmp ((*a)->ext, (*b)->ext);
            if (i == 0)
                return (0);
            else
                return (i);
        } else
            return (i);
    } else
        return (i);
};

static int my_cmp3 (Scripts_ptr * a, Scripts_ptr * b) {

    if ((*a)->ttr == (*b)->ttr) {
        return (0);
    } else if ((*a)->ttr > (*b)->ttr)
        return (1);
    else
        return (-1);
};

//
// The 'code' (was:interpreter) this function name is 'registered' along with Admin_init/Admin_exit...
//
void Admin_run (void * me, SERVER * sp, APPLICA * ap, SESSION * np, REQUEST * rp, RESPONS * pp, OBJECTC * op, VBERROR * ip) {
    long          i;
    long          j;
    Scripts_ptr * sca; // Sort array - dynamically allocated from the heap...
    Scripts_ptr   scp;
    VARIANT     * v;
    char        * s;
    char        * t;
    char        * Command;
    char        * Path = NULL;
    char        * File = NULL;
    char        * Extn = NULL;
    char        * Init = NULL;
    char        * Exit = NULL;
    char        * Run = NULL;
    long          sort_by = 0; // Sort by Path - Name - Ext initially...

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

    Response->Write ("<HTML><BODY><FORM NAME=\"form1\" METHOD=POST ACTION=\"admin.asp\" AUTOCOMPLETE=OFF>\n"); // Set up text output to a browser document...

    Response->Write ("Welcome to the admin page.  (Max. Heap Used: %d bytes (%08X), Max. Stack Used: %d bytes (%08X))<p>\n",
        __libc_heap_max, __libc_heap_max, __libc_stack_max, __libc_stack_max);

    // Process any requests before generating anymore HTML
    v = Request->Default ("Command", 3); // (3) = Only look in Form & QueryString Collections - skip Cookies and ServerVariables.
    if ((v) && (v->SubType == 1)) { // Is a String *

        Command = ((STRING *)(v->Value.ov))->Value;

        do { // An easy escape...

            v = Request->Default ("Path", 3);
            if ((v) && (v->SubType == 1))
                Path = ((STRING *)(v->Value.ov))->Value;
            else
                break; // Error... Abort this section.
            
            v = Request->Default ("File", 3);
            if ((v) && (v->SubType == 1))
                File = ((STRING *)(v->Value.ov))->Value;
            else
                break; // Error... Abort this section.
            
            v = Request->Default ("Extn", 3);
            if ((v) && (v->SubType == 1))
                Extn = ((STRING *)(v->Value.ov))->Value;
            else
                break; // Error... Abort this section.

            v = Request->Default ("Init", 3);
            if ((v) && (v->SubType == 1))
                Init = ((STRING *)(v->Value.ov))->Value;
            else
                break; // Error... Abort this section.
            
            v = Request->Default ("Exit", 3);
            if ((v) && (v->SubType == 1))
                Exit = ((STRING *)(v->Value.ov))->Value;
            else
                break; // Error... Abort this section.
            
            v = Request->Default ("Run", 3);
            if ((v) && (v->SubType == 1))
                Run = ((STRING *)(v->Value.ov))->Value;
            else
                break; // Error... Abort this section.

            if (strcmp (Command, "BUILD") == 0) {

                zap_Script (Path, File, Extn);
                Response->Write ("Zapped \"%s%s.%s\"...<p>\n", Path, File, Extn);

            } else if (strcmp (Command, "DELETE") == 0) {

                if (del_Script (Path, File, Extn))
                    Response->Write ("Deleted \"%s%s.%s\".<p>\n", Path, File, Extn);
                else
                    Response->Write ("Deletion of \"%s%s.%s\" failed.<p>\n", Path, File, Extn);

            } else if (strcmp (Command, "ADD") == 0) {

                if ((Path [0] == 0) ||
                    (File [0] == 0) ||
                    (Extn [0] == 0) ||
                    (Init [0] == 0) ||
                    (Exit [0] == 0) ||
                    (Run [0] == 0)) {

                    Response->Write ("Registration requires all fields to be entered.<p>\n");
                    
                    break; // Don't process incomplete entries...
                };

                // check Path has / on both ends!
                if ((Path [0] != '/') ||
                    (Path [strlen (Path) - 1] != '/')) {

                    Response->Write ("A HTTP Path requires '/' on both ends.<p>\n");
                    
                    break; // Don't process incomplete entries...
                };

                if (add_Script (Path, File, Extn, Init, Exit, Run))
                    Response->Write ("Registered \"%s%s.%s\".<p>\n", Path, File, Extn);
                else
                    Response->Write ("Registration of \"%s%s.%s\" failed.<p>\n", Path, File, Extn);

            } else if (strcmp (Command, "REFRESH") == 0) {

                Response->Write ("* READY *<p>\n");

            } else if (strcmp (Command, "SAVE") == 0) {

                // Pick-Up the MVS settings

                v = Request->Default ("UserSources", 3);
                if ((v) && (v->SubType == 1) && (s = ((STRING *)(v->Value.ov))->Value) && (strcmp (wp->user_sources, s) != 0)) {
                    t = wp->user_sources;
                    wp->user_sources = (char *)malloc (strlen (s) + 1);
                    if (wp->user_sources) {
                        strcpy (wp->user_sources, s);
                        free (t);
                    } else
                        wp->user_sources = t;
                };

                v = Request->Default ("UserHeaders", 3);
                if ((v) && (v->SubType == 1) && (s = ((STRING *)(v->Value.ov))->Value) && (strcmp (wp->user_headers, s) != 0)) {
                    t = wp->user_headers;
                    wp->user_headers = (char *)malloc (strlen (s) + 1);
                    if (wp->user_headers) {
                        strcpy (wp->user_headers, s);
                        free (t);
                    } else
                        wp->user_headers = t;
                };

                v = Request->Default ("JCCLHeaders", 3);
                if ((v) && (v->SubType == 1) && (s = ((STRING *)(v->Value.ov))->Value) && (strcmp (wp->jccl_headers, s) != 0)) {
                    t = wp->jccl_headers;
                    wp->jccl_headers = (char *)malloc (strlen (s) + 1);
                    if (wp->jccl_headers) {
                        strcpy (wp->jccl_headers, s);
                        free (t);
                    } else
                        wp->jccl_headers = t;
                };

                // Pick-Up any new WebServer details here too:

                v = Request->Default ("ServerIndex", 3);
                if ((v) && (v->SubType == 1) && (s = ((STRING *)(v->Value.ov))->Value) && (server_index != (i = atoi (s))))
                    server_index = i;

                v = Request->Default ("WebServer", 3);
                if ((v) && (v->SubType == 1) && (s = ((STRING *)(v->Value.ov))->Value) && (strcmp (wp->name, s) != 0)) {
                    t = wp->name;
                    wp->name = (char *)malloc (strlen (s) + 1);
                    if (wp->name) {
                        strcpy (wp->name, s);
                        free (t);
                    } else
                        wp->name = t;
                };

                v = Request->Default ("Description", 3);
                if ((v) && (v->SubType == 1) && (s = ((STRING *)(v->Value.ov))->Value) && (strcmp (wp->description, s) != 0)) {
                    t = wp->description;
                    wp->description = (char *)malloc (strlen (s) + 1);
                    if (wp->description) {
                        strcpy (wp->description, s);
                        free (t);
                    } else
                        wp->description = t;
                };

                v = Request->Default ("Binding", 3);
                if ((v) && (v->SubType == 1) && (s = ((STRING *)(v->Value.ov))->Value) && (strcmp (wp->open_card, s) != 0)) {
                    t = wp->open_card;
                    wp->open_card = (char *)malloc (strlen (s) + 1);
                    if (wp->open_card) {
                        strcpy (wp->open_card, s);
                        free (t);
                    } else
                        wp->open_card = t;
                };

                v = Request->Default ("Port", 3);
                if ((v) && (v->SubType == 1) && (s = ((STRING *)(v->Value.ov))->Value) && (wp->open_port != (i = atoi (s))))
                    wp->open_port = i;

                v = Request->Default ("Threads", 3);
                if ((v) && (v->SubType == 1) && (s = ((STRING *)(v->Value.ov))->Value) && (wp->worker_threads != (i = atoi (s))))
                    wp->worker_threads = i;

                v = Request->Default ("MSess", 3);
                if ((v) && (v->SubType == 1) && (s = ((STRING *)(v->Value.ov))->Value) && (wp->sessions_max != (i = atoi (s))))
                    wp->sessions_max = i;

                v = Request->Default ("TSess", 3);
                if ((v) && (v->SubType == 1) && (s = ((STRING *)(v->Value.ov))->Value) && (wp->session_t_o != (i = atoi (s))))
                    wp->session_t_o = i;

                v = Request->Default ("Logging", 3);
                if ((v) && (v->SubType == 1) && (s = ((STRING *)(v->Value.ov))->Value) && (wp->logging != (i = atoi (s))))
                    wp->logging = i;

//                v = Request->Default ("Logswitch", 3);
//                if ((v) && (v->SubType == 1) && (s = ((STRING *)(v->Value.ov))->Value) && (wp->logswitch != (i = atoi (s))))
//                    wp->logswitch = i;

                v = Request->Default ("Logfile", 3);
                if ((v) && (v->SubType == 1) && (s = ((STRING *)(v->Value.ov))->Value) && (strcmp (wp->logdir, s) != 0)) {
                    t = wp->logdir;
                    wp->logdir = (char *)malloc (strlen (s) + 1);
                    if (wp->logdir) {
                        strcpy (wp->logdir, s);
                        free (t);
                    } else
                        wp->logdir = t;
                };

                v = Request->Default ("Logopts", 3);
                if ((v) && (v->SubType == 1) && (s = ((STRING *)(v->Value.ov))->Value) && (wp->logopts != (i = strtol (s, NULL, 16))))
                    wp->logopts = i;

                // Respond...
                Response->Write ("* SAVED *<p>\n");

                wp->save_disable = 0; // Can override the disable now the user wants to save...
                wp->save_trigger = 1; // Actually do a save when this transaction is complete.

            } else if (strcmp (Command, "QUIT") == 0) {
                
                Response->Write ("* QUIT *\n");

                wp->save_disable = 1; // Can override the disable to protect - user wants to abort.
                running = 0;

                goto ASP_exit;

            } else {

                Response->Write ("Invalid Command %s.<p>\n", Command);
            };

        } while (0); // ...Never actually loop back.

    } else {

        Response->Write ("* READY *<p>\n");
    };

    // Process form/query variables needed...
    v = Request->Default ("SortBy", 3);
    if ((v) && (v->SubType == 1)) // Is a String *
        sort_by = atoi (((STRING *)(v->Value.ov))->Value);

    // Start generating more HTML...
    Response->Write ("<u><b>WebServer Settings:</b></u><BR>\n");

    Response->Write ("<TABLE RULES=NONE BGCOLOR=LIGHTGREY WIDTH=100%%><TR><TD>User Sources:</TD><TD>");
    Response->Write ("<INPUT ID=UserSources NAME=UserSources TYPE=TEXT SIZE=50 VALUE=\"%s\">\n", wp->user_sources);

    Response->Write ("</TD></TR><TR><TD>User Headers:</TD><TD>");
    Response->Write ("<INPUT ID=UserHeaders NAME=UserHeaders TYPE=TEXT SIZE=50 VALUE=\"%s\">\n", wp->user_headers);
    
    Response->Write ("</TD></TR><TR><TD>JCC Library Headers:</TD><TD>");
    Response->Write ("<INPUT ID=JCCLHeaders NAME=JCCLHeaders TYPE=TEXT SIZE=50 VALUE=\"%s\">\n", wp->jccl_headers);
    
    Response->Write ("</TD></TR><TR><TD>Server-Session Instance:</TD><TD>");
    Response->Write ("<INPUT ID=ServerIndex NAME=ServerIndex TYPE=TEXT SIZE=50 VALUE=\"%d\">\n", server_index);
    
    Response->Write ("</TD></TR><TR><TD>WebServer:</TD><TD>");
    Response->Write ("<INPUT ID=WebServer NAME=WebServer TYPE=TEXT SIZE=50 VALUE=\"%s\">\n", wp->name);

    Response->Write ("</TD></TR><TR><TD>Description:</TD><TD>");
    Response->Write ("<INPUT ID=Description NAME=Description TYPE=TEXT SIZE=50 VALUE=\"%s\">\n", wp->description);
    
    Response->Write ("</TD></TR><TR><TD>NetWork IP Binding ('any' means all cards):</TD><TD>");
    Response->Write ("<INPUT ID=Binding NAME=Binding TYPE=TEXT SIZE=50 VALUE=\"%s\">\n", wp->open_card);
    Response->Write ("</TD></TR><TR><TD>Port:</TD><TD>");
    Response->Write ("<INPUT ID=Port NAME=Port TYPE=TEXT SIZE=50 VALUE=\"%d\">\n", wp->open_port);

    Response->Write ("</TD></TR><TR><TD>Worker Threads (0 ensures immediate processing):</TD><TD>");
    Response->Write ("<INPUT ID=Threads NAME=Threads TYPE=TEXT SIZE=50 VALUE=\"%d\">\n", wp->worker_threads);

    Response->Write ("</TD></TR><TR><TD>Max. Sessions (0 means unlimited):</TD><TD>");
    Response->Write ("<INPUT ID=MSess NAME=MSess TYPE=TEXT SIZE=50 VALUE=\"%d\">\n", wp->sessions_max);

    Response->Write ("</TD></TR><TR><TD>Session Timeout (Mins.  Only when worker-threads > 0):</TD><TD>");
    Response->Write ("<INPUT ID=TSess NAME=TSess TYPE=TEXT SIZE=50 VALUE=\"%d\">\n", wp->session_t_o);

    Response->Write ("</TD></TR><TR><TD>Logging (0 = Off, 1 = On):</TD><TD>");
    Response->Write ("<INPUT ID=Logging NAME=Logging TYPE=TEXT SIZE=50 VALUE=\"%d\">\n", wp->logging);

//    Response->Write ("</TD></TR><TR><TD>Log file-switching (0=Sequential,1=Hr,2=Day,3=We,4=Mo,5=<#bytes):</TD><TD>");
//    Response->Write ("<INPUT ID=Logswitch NAME=Logswitch TYPE=TEXT SIZE=50 VALUE=\"%d\">\n", wp->logswitch);

    Response->Write ("</TD></TR><TR><TD>Log File/Directory:</TD><TD>");
    Response->Write ("<INPUT ID=Logfile NAME=Logfile TYPE=TEXT SIZE=50 VALUE=\"%s\">\n", wp->logdir);

    Response->Write ("</TD></TR><TR><TD>Log Option flags (32 Bit Hex. value):</TD><TD>");
    Response->Write ("<INPUT ID=Logopts NAME=Logopts TYPE=TEXT SIZE=50 VALUE=\"%08X\">\n", wp->logopts);

    Response->Write ("</TD></TR></TABLE>\n");

    Response->Write ("<INPUT TYPE=BUTTON NAME=Terminate VALUE=Terminate onclick=\"Command.value='QUIT';form1.submit();\"> the WebServer (no save).&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\n");
    Response->Write ("<INPUT TYPE=BUTTON NAME=Save VALUE=Save onclick=\"Command.value='SAVE';form1.submit();\"> the current settings (including: <b>registrations</b> and deletions.)<p>\n");

    // 'Place' the hidden variables:
    Response->Write ("<INPUT ID=Command NAME=Command TYPE=HIDDEN VALUE=\"\">\n");
    
    Response->Write ("<u><b>New Registrations:</b></u><BR>\n");

    Response->Write ("<TABLE RULES=NONE BGCOLOR=LIGHTGREY WIDTH=100%%><TR><TD>VirtualPath:</TD><TD>");
    if (Path)
        Response->Write ("<INPUT ID=Path NAME=Path TYPE=TEXT VALUE=\"%s\">\n", Path);
    else
        Response->Write ("<INPUT ID=Path NAME=Path TYPE=TEXT VALUE=\"\">\n");

    Response->Write ("</TD><TD>_init:</TD><TD>");
    if (Init)
        Response->Write ("<INPUT ID=Init NAME=Init TYPE=TEXT VALUE=\"%s\">\n", Init);
    else
        Response->Write ("<INPUT ID=Init NAME=Init TYPE=TEXT VALUE=\"\">\n");

    Response->Write (" (these _3 are case-sensitive.)</TD></TR><TR><TD>Filename:</TD><TD>");
    if (File)
        Response->Write ("<INPUT ID=File NAME=File TYPE=TEXT VALUE=\"%s\">\n", File);
    else
        Response->Write ("<INPUT ID=File NAME=File TYPE=TEXT VALUE=\"\">\n");

    Response->Write ("</TD><TD>_exit:</TD><TD>");
    if (Exit)
        Response->Write ("<INPUT ID=Exit NAME=Exit TYPE=TEXT VALUE=\"%s\">\n", Exit);
    else
        Response->Write ("<INPUT ID=Exit NAME=Exit TYPE=TEXT VALUE=\"\">\n");

    Response->Write ("</TD></TR><TR><TD>Extension:</TD><TD>");
    if (Extn)
        Response->Write ("<INPUT ID=Extn NAME=Extn TYPE=TEXT VALUE=\"%s\">\n", Extn);
    else
        Response->Write ("<INPUT ID=Extn NAME=Extn TYPE=TEXT VALUE=\"\">\n");

    Response->Write ("</TD><TD>_run:</TD><TD>");
    if (Run)
        Response->Write ("<INPUT ID=Run NAME=Run TYPE=TEXT VALUE=\"%s\">\n", Run);
    else
        Response->Write ("<INPUT ID=Run NAME=Run TYPE=TEXT VALUE=\"\">\n");

    Response->Write ("</TD></TR></TABLE>\n");

    Response->Write ("<INPUT ID=SortBy NAME=SortBy TYPE=HIDDEN VALUE=\"%d\">\n", sort_by);

    // The java-code required to auto-fill these hidden variables: onclick=\"Command.value='BUILD';Path.value='';File.value='';Extn.value='';form1.submit();\"

    // Add 'Other' Items...
    Response->Write ("<INPUT TYPE=BUTTON NAME=Register VALUE=Register onclick=\"Command.value='ADD';form1.submit();\"> these values as a new virtual page.<p>\n");

    // List the registered scripts and thier details... (plus buttons for 'rebuilding')
    Response->Write ("<u><b>Currently registered objects:</b></u>&nbsp;&nbsp;&nbsp;\n");

    Response->Write ("<INPUT TYPE=BUTTON NAME=SortByPath VALUE=SortByPath onclick=\"Command.value='REFRESH';SortBy.value='0';form1.submit();\">\n");
    Response->Write ("<INPUT TYPE=BUTTON NAME=SortByFile VALUE=SortByFile onclick=\"Command.value='REFRESH';SortBy.value='1';form1.submit();\">\n");
    Response->Write ("<INPUT TYPE=BUTTON NAME=SortByTTRz VALUE=SortByTTRz onclick=\"Command.value='REFRESH';SortBy.value='2';form1.submit();\">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\n");

    Response->Write ("<INPUT TYPE=BUTTON NAME=Refresh VALUE=Refresh onclick=\"Command.value='REFRESH';form1.submit();\"><BR>\n");

    Response->Write ("<TABLE BORDER BGCOLOR=LIGHTGREY WIDTH=100%%>");
    Response->Write ("<TR><TD BGCOLOR=LIGHTGREY>Path</TD><TD>File</TD><TD BGCOLOR=LIGHTGREY>Ext</TD><TD>_init</TD><TD>_exit</TD><TD>_run</TD><TD BGCOLOR=GRAY>handle</TD><TD BGCOLOR=GRAY>_init_adx</TD><TD BGCOLOR=GRAY>_run_adx</TD><TD>TTRz</TD><TD BGCOLOR=RED>&nbsp;</TD><TD BGCOLOR=BLUE>&nbsp;</TD></TR>\n");

    i = 0;
    scp = shead;
    while (scp) {
        i++;
        scp = scp->next;
    };
    if (i) {
        sca = (Scripts_ptr *)malloc (i * 4);
        if (sca) {

            j = 0;
            scp = shead;
            while (j < i) {
                sca [j++] = scp;
                scp = scp->next;
            };
            // Now we have an array of pointers...
            if (sort_by == 0)
                qsort (sca, i, 4, (void *)&my_cmp1); // Sorted by Path - Name - Ext
            else if (sort_by == 1)
                qsort (sca, i, 4, (void *)&my_cmp2); // Sorted by Name - Path - Ext
            else
                qsort (sca, i, 4, (void *)&my_cmp3); // Sorted by TTR

            j = 0;
            while (j < i) {

                if (sca [j]->dynobp == NULL) {
                    Response->Write ("<TR><TD BGCOLOR=LIGHTGREY>%s</TD><TD BGCOLOR=WHITE>%s</TD><TD BGCOLOR=LIGHTGREY>%s</TD><TD BGCOLOR=WHITE>%s</TD><TD BGCOLOR=WHITE>%s</TD><TD BGCOLOR=WHITE>%s</TD><TD BGCOLOR=LIGHTGREY>Has - Never -</TD><TD BGCOLOR=LIGHTGREY>Been -</TD><TD BGCOLOR=LIGHTGREY>Accessed</TD><TD BGCOLOR=PINK>Waiting to build</TD>"
                        "<TD><INPUT TYPE=BUTTON NAME=Delete VALUE=Delete onclick=\"Command.value='DELETE';Path.value='%s';File.value='%s';Extn.value='%s';Init.value='%s';Exit.value='%s';Run.value='%s';form1.submit();\"></TD>\n"
                        "<TD>&nbsp;</TD></TR>",
                                          sca [j]->path, sca [j]->name, sca [j]->ext, sca [j]->init, sca [j]->exit, sca [j]->arun,
                                          sca [j]->path, sca [j]->name, sca [j]->ext, sca [j]->init, sca [j]->exit, sca [j]->arun);
                } else {
                    if (sca [j]->ttr == 0) {
                        Response->Write ("<TR><TD BGCOLOR=LIGHTGREY>%s</TD><TD BGCOLOR=WHITE>%s</TD><TD BGCOLOR=LIGHTGREY>%s</TD><TD BGCOLOR=WHITE>%s</TD><TD BGCOLOR=WHITE>%s</TD><TD BGCOLOR=WHITE>%s</TD><TD BGCOLOR=GRAY>0x%08X</TD><TD BGCOLOR=GRAY>0x%08X</TD><TD BGCOLOR=GRAY>0x%08X</TD><TD BGCOLOR=PINK>Waiting to build</TD>"
                            "<TD><INPUT TYPE=BUTTON NAME=Delete VALUE=Delete onclick=\"Command.value='DELETE';Path.value='%s';File.value='%s';Extn.value='%s';Init.value='%s';Exit.value='%s';Run.value='%s';form1.submit();\"></TD>\n"
                            "<TD>&nbsp;</TD></TR>",
                                              sca [j]->path, sca [j]->name, sca [j]->ext, sca [j]->init, sca [j]->exit, sca [j]->arun, sca [j]->dynobp, sca [j]->obj, sca [j]->run,
                                              sca [j]->path, sca [j]->name, sca [j]->ext, sca [j]->init, sca [j]->exit, sca [j]->arun);
                    } else {
                        Response->Write ("<TR><TD BGCOLOR=LIGHTGREY>%s</TD><TD BGCOLOR=WHITE>%s</TD><TD BGCOLOR=LIGHTGREY>%s</TD><TD BGCOLOR=WHITE>%s</TD><TD BGCOLOR=WHITE>%s</TD><TD BGCOLOR=WHITE>%s</TD><TD BGCOLOR=GRAY>0x%08X</TD><TD BGCOLOR=GRAY>0x%08X</TD><TD BGCOLOR=GRAY>0x%08X</TD><TD BGCOLOR=LIGHTGREY>0x%08X</TD>"
                            "<TD><INPUT TYPE=BUTTON NAME=Delete VALUE=Delete onclick=\"Command.value='DELETE';Path.value='%s';File.value='%s';Extn.value='%s';Init.value='%s';Exit.value='%s';Run.value='%s';form1.submit();\"></TD>\n"
                            "<TD><INPUT TYPE=BUTTON NAME=ReBuild VALUE=ReBuild onclick=\"Command.value='BUILD';Path.value='%s';File.value='%s';Extn.value='%s';Init.value='%s';Exit.value='%s';Run.value='%s';form1.submit();\"></TD></TR>\n",
                                              sca [j]->path, sca [j]->name, sca [j]->ext, sca [j]->init, sca [j]->exit, sca [j]->arun, sca [j]->dynobp, sca [j]->obj, sca [j]->run, sca [j]->ttr,
                                              sca [j]->path, sca [j]->name, sca [j]->ext, sca [j]->init, sca [j]->exit, sca [j]->arun,
                                              sca [j]->path, sca [j]->name, sca [j]->ext, sca [j]->init, sca [j]->exit, sca [j]->arun);
                    };
                };
                j++;
            };

            free (sca);
        };
    };

    Response->Write ("</TABLE>");
    Response->Write ("<b>Warning:</b>New registrations are <b>not</b> automatically saved.<p>");

ASP_exit:

    Response->Write ("</FORM></BODY></HTML>\n"); // Errors always close the HTML document in this script.

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
