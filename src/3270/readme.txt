The 3270 panel designer:
------------------------

Is a visual basic application (VB runtime not included due to lack of
space available in the JCC distribution package) which allows the
design and generation of C code to produce 3270 TSO panels.

See example.c for a complete example with documentation of how the parts
work together.

test1.d37 is the template definition which the des3270.exe program uses
to generate the test1.h C include file.  This file has the code required
to build a 3270 screen at runtime, in the example program.  The function
within test1.h which outputs & inputs a 3270 screen is called test1.

ASMSCAN/OBJSCAN may be used to assemble the .ASM files in MVS3.8j, but
they are already included as objects in this directory.


Fri 9th Jan 2009 Update
-----------------------

There is a new version of the generator that now allows the style and colour
attributes to be overridden at run-time, by using a similar array of 2 byte
strings to the arrays already used for input/output variables.  You may pass
NULL to each of the two new function parameters: oattrs and ioattrs if you
don't want to use this new option.


About the VB runtime:
---------------------
comdlg32.ocx
richtx32.ocx
mscomctl.ocx

are the components required by des3270.exe, along with:

asycfilt.dll
comcat.dll
msvbvm60.dll
oleaut32.dll
olepro32.dll

These files represent over 3MB of additional space required, and are
available elsewhere...  NT service packs for example.  Usually only
Win95/Win98 do not have these files already installed.
