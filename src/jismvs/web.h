/*
 * JIS MVS3.8j WWW Application Server.  (C)2004 Jason Paul Winter, All Rights Reserved.
 *
 * jasonwinter@hotmail.com, use "Subject" wisely - spam gets deleted without being read.
 *
 */

/*** Web Server Settings ***/

// The maximum number of worker threads allowed (1 per CPU is OK.)
// JCC Limit = 256.
// ...64 also limits the number of sockets used to 65.
#define MaxThreads 64

#define default_port 80
#define ini_file "//DDN:INIFILE"

//
// Each 'script' needs to contain an entry point function
// and accept a parameter list of the WebServer Objects
// which are defined in "ASP.H" for general WebServer use.
//

typedef struct Scripts_tag * Scripts_ptr;
typedef struct Scripts_tag {
    char    *   path;   // WebServer path to access a given member
    char    *   name;   // member name
    char    *   ext;    // Assigned extension name
    char    *   init;   // Name of the Class-Object init function
    char    *   exit;   // Name of the Class-Object exit function
    char    *   arun;   // Name of the Class-Object script-run function
    void    *   dynobp; // _jcc_load'ed handle so it can be deleted...
    void    *   obj;    // The object 'type' (pointer to '_init' function)
    void    * (*run)(); // The function address for '_run' function
    long        ttr;    // where from exactly?
    // *** Warning *** - A compress or recompile will result in a full-auto-reload.

    Scripts_ptr next;
} Scripts;

typedef struct WebSite_tag * WebSite_ptr;
typedef struct WebSite_tag {

    // Interactions with 'admin'
    int    save_disable;    // 1 if the system ini file isn't one of ours.
    int    save_trigger;    // 1 if a save is needed

    // MVS - filenamesm where initial mode = DDs until first-save.
    char * user_sources;    // "//DDN:ASPSRCS" or "//DSN:ASP.SOURCE" etc.
    char * user_headers;    // "//DDN:ASPSRCS" or "//DSN:ASP.SOURCE" etc.
    char * jccl_headers;    // "//DDN:JCCINCL" or "//DSN:JCC.INCLUDE"

    // Web Site
    char * name;            // "www.jccompiler.com" - entered in admin.
    char * description;
    char * open_card;       // Network IP of physical-card (or 'any')
    int    open_port;
    int    worker_threads;  // 0 is the default 64 is max.

    // Session control
    int    sessions_max;    // Number of Sessions before new requests are rejected.
    int    session_t_o;     // Session timeout in minutes

    // Logging switch
    int    logging;         // (0=no, 1=W3C)

    // Logging Properties
    int    logswitch;//***NA// (0=no-seq, 1-hr,2-da,3-we,4=mo,5=< bytes!)
    char * logdir;          // file members are in (JLyymmdd) format
    long   logopts;         // bitmask for the following properties:
} WebSite;

/* W3C mode logging:
d Logging Properties (d-default .=yes else no)
  ------------------
  Date
. Time
. Client IP Address
  User Name
  Service Name
  Server Name
  Server IP Address
  Server Port
. Method
. URI Stem
  URI Query
. Protocol Status
  Bytes Sent
  Bytes Received
  Time Taken
  Protocol Version
  Host
  User Agent
  Cookie
  Referer

  defaults = 0110 0000 1101 0000 0000 0000 0000 0000
           = 60D00000

#Software: JCC-ASP1.0
#Version: 1.0
#Date: 2003-12-04 01:47:43
#Fields: time c-ip cs-method cs-uri-stem sc-status 
01:47:43 127.0.0.1 GET /default.cfm 200
01:47:43 127.0.0.1 GET /main.css 404

/*** End ***/
