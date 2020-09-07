08 Jan 2009
-----------

Files in this package:
----------------------

<Root>
------
README.TXT   : This file!

RESET.C      : A quick reset of *all* the allocated sockets, when developing.

PORTS.C      : A quick method of telling which sockets are connected to where.

EMAIL.C      : A simple interface to smtp for linking with new programs or use in batch.

FTPD.C       : Source to FTP-MVS38j, it will shutdown if the 'reset' program (above) is executed.
MVSDIRS.H    : C/ASM-code (in a header) for reading VATLST00, calling IEHLIST and defining LISTER.
FTPCMD.C     : Source to control FTP-MVS38j depending on the build options used.

JCL
---
FTP.JCL      : A PROCLIB member to start FTP via the console ("S FTP") under MVS38j.
EMAIL.JCL    : Sample JCL to send an email, you can use ComputerName:25 for smtp if DNS is working.

MVS
---
EZASOKET.ASM : The RENT Socket Driver source code for standard linkage, a .OBJ comes with JCC.

Note: FTP will also shutdown cleanly if a TELNET-ANSI client on Port:21 sends "term" or "terminate"


How to 'make':
--------------

1/ Compile RESET.C with JCC and pre/link into LINKLIB.
1a/ ditto  PORTS.C
1b/ ditto  EMAIL.C
1c/ ditto  FTPCMD.C
2/ Compile FTPD.C with JCC and link into LINKLIB (but as FTP.)
3/ At the MASTER CONSOLE execute "S FTP" to start the new PROCLIB member for the FTP-MVS38j Daemon.

Now you can connect to the FTP-MVS38j Daemon using whatever FTP Client you like and read/write files.
Note: Although all files will be displayed, only cataloged files can be accessed.

6/ Run RESET from batch, TSO-Call or even just as a TSO command processor to shutdown all sockets.
Note: This should/will shutdown every socket using program - useful when closing down Hercules.

6a/ Alternatively run the FTPCMD program (with 1, but any parameter) to only shutdown the FTP Daemon.

More info:
----------

MVS38j FTPD - Updated Source Code (17-Dec-2008)

This version of the FTP server refreshes the file list when it detects that
the user has accessed a previously unknown file, as well as when a root
directory listing is requested.  Many FTP clients already use a locally
built cache, so most of the time this doesn't add too much processing overhead.

You may comment out the refresh directory listing option by searching for:

"Comment-out for faster directory refreshes (using the cache)"

And putting two slashes in front of the refresh (); statement found.  This
would be useful when running the server on a slower machine.  To force the
server to refresh its directory listing cache, use the provided ftpcmd.c
program without any parameters.  Running the ftpcmd.c program with a parameter
(anything) will request the FTP server to 'cleanly' stop running.  The older
reset.c program also does this, but it stops all other TCP/IP applications too.
