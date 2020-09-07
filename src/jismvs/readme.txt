                            *** JCC Information Server ***
                                    *** V1.0 ***
                         *** (MVS Web Application Server) ***

What is it?
-----------
A Web Server with user-partitioned 'session' storage held across 'transaction' requests
which uses an Object Oriented design to provide memory leak protection and 'logical order.'
A "script engine" of the C language, with 'just-in-time' compiling and dynamic (re)loading
which bypasses the need for an interpreter, and scales with the number of CPUs the machine
has by using the JCC multi-threading server model.

Why is that good?
-----------------
Having the Web Server on the same machine as a database server means data-traffic sent
between internal applications is much quicker than when a *real* network wire is used.
ie. The system can drive Web front-ends to server-socket back-end database application modules
(running under CICSWeb/COBOL or a similar technology) which all run on the same machine
to improve performance.  Add to this the just-in-time compiled C script technology and web
application performance problems should be a thing of the past.

What isn't it?
--------------
A file server!  Use a PC based Web Server to serve graphics and other documents.  This
effectively offloads bulky static page data, leaving the Application Server (and its network
path) to respond to real-work only.  To serve files - the error script should be altered so
404 Not Found errors are referenced against a database of files that *may* be served.

A smart proxy may be used to join a number of web servers into one large virtual web server,
which would allow 'self-referencing' graphic URLs to be generated in your Web Applications.

Licenses!
---------
You must have a valid license to the JCC compiler and library to legally use this software.

There are no ongoing fees to use (and enhance for your own use) this Web Server software,
however if you are using it for business (ie. not with Hercules and MVS3.8j) you must pay a
one-off fee of US$750 plus any other ongoing costs:

The script engine does not fall under the normal free-use JCC license agreement, which is
designed to allow you to freely compile other peoples free ANSI C programs.  In this
system, the programs being compiled are - your own - for your own use - for use by others
on your behalf etc.  The program-scripts are also not ANSI C (it's a script!  Kind-of.)

Apart from a current JCC compiler and library license, the only other restriction on usage
is that the system may not be repackaged, and/or distributed under a different product name.
This means new applications built on top of this system have to be distributed separately.

How to set it up:
-----------------
The batch files that came in the zip package build the MVS object deck (like any JCC program.)

It needs to run in it's own address space (as a started task, from the master console)
although it will run in any environment where (the default) TCPIP Port 80 can be opened...

This includes Batch and TSO, but at the minimum the following is required:

. An empty INI file which can be used to save server settings. (ref. INIFILE)
. Access to the JCC compiler in the linklist.
. Access to JCC library include files. (ref. JCCINCL, VB8188/8192 for all sources:)
. Access to JCC C Scripts. (ref. ASPSRCS - TTRs are recorded to detect possible updates.)
. Access to JCC C Script Headers. (ref. JCCINCS)

Objects that make up the Web Application Server have a Class-Header file which is generated
for external-access (performed automatically by the batch files.)  These header files must
be kept up-to-date (if any design changes are implemented) for the just-in-time compile step
to read and build correctly linking modules.  Although scripts are themselves considered
Objects by the JCC library, they are still mostly 'external' and do not require this special
Class-Header file.  (Copy these header files with asp.h and web.h to 'ASP.HEADERS'.)

When JCC accesses member files (reading header files) it truncates them at the decimal point
or after the 8th character.  Some header files in this system are 'too long' and must be
stored as members with the same truncation.  (The batch files also renames those files.)

eg. JCL:

//JIS      JOB  JIS,CLASS=A,MSGCLASS=A,MSGLEVEL=(1,1)
//*
//* Allocate enough memory for the number of users expected.
//*
//RUNSTEP  EXEC PGM=JIS,REGION=8192K
//STDOUT   DD   SYSOUT=*
//*
//* Initially an empty file (Fixed Block) used to save settings.
//INIFILE  DD   DSN=ASP.INI,DISP=SHR
//*
//* It's best to leave this as a DD (write locking isn't a problem.)
//JCCINCL  DD   DSN=JCCINC,DISP=SHR
//*(To allow writing, DDs are not allowed - they lock the file.)
//* Comment out these lines after setting up in admin with '//DSN:'
//ASPSRCS  DD   DSN=ASP.SOURCES,DISP=SHR
//JCCINCS  DD   DSN=ASP.HEADERS,DISP=SHR
//*
//* Logging is *off* by default.  (This is optional.)
//*LOGDIR   DD   DSN=ASP.LOG,DISP=SHR

The rest of the setup needs to be done within a browser, while the system is running.

The main.c source file can be altered so temporary files are not allocated, and 3 extra
DD statements will be required.  This may aid in running the system on VM instead of MVS.

Two scripts come pre-initialised in a new Web Application Server instance, the error script
which captures all unexpected exceptions and the admin script.  There is a third script
called 'hello' which is designed as a Hello World example for the Web Server.

The system allows itself to configure new applications through a web interface in the admin
application, which comes as a real-example of how it all should work:

HTTP://127.0.0.1/admin.asp

Only the error page is really needed for a working system, and the admin page may be
unregistered to prevent a known access point being available for hacking.  Be warned however
that re-adding the admin page needs to be coded in an existing application for recovery!

To allow write-access to ASP.SOURCES from other started-tasks, the DD must be removed from
the JCL after the first full execution where visiting the admin page allows the correct DSN
style dataset names to be entered so no dataset lock is held unless the compiler is working.

If you do not intend to update C scripts while this system is operational, then leaving the
DD statements as-is will also work, and can be used to see the working example.

To access the hello script: Register the following details...

Path "/"
File "hello"
Ext  "asp"
_init "Hello_init"
_exit "Hello_exit"
_run  "Hello_run"

and press the "Register" button,
and press the "Save" button (to ensure "/hello.asp" is available next time the server starts.)

Once HTTP://127.0.0.1/hello.asp is accessed, the compiler will kick-in and you will see the
output.  If the TTRz of the file is altered (new member contents) then a flag will be set to
mark the file for recompiling/reloading on the next URL access.

                  (C)2004 Jason Paul Winter, All Rights Reserved.
