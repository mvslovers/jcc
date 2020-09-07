# JCC Compiler

## Important Note

This is a fork(-ish) from the source found on tk4- under `TK4-.JASON.JCC.ZIP` downloaded with wget: `wget ftp://her01:cul8tr@localhost:21021/TK4-.JASON.JCC.ZIP` and unzipped on linux with `unzip TK4-.JASON.JCC.ZIP`. I am not the creator nor maintainer of JCC. See LICENSE.MD for more information. 

## About

JCC is an ANSI compliant C Compiler that generates '370 assembly language.

JCC uses the JCC-Library which has optimisations for '390 processors which are automatically switched on when running on that equipment. For Hercules users running MVS3.8j who have compiled Hercules with 390 extensions-enabled, there is a global variable to allow manual switching to the faster library code.

Utilities that come with JCC:

### asmscan
Replaces identifiers in assembly listings with STnnnnn where n is a unique 5 digit number. This has the effect of allowing long-names within C programs to be assembled with older assemblers. ALIAS statements are also no longer required, and are removed along with AMODE & RMODE directives.

A mapping file is generated by asmscan to allow the new short names to be remapped back to their original names. It is intended to be used as input to objscan.

Warning: To use this utility, the source file must not contain global variables or function names in this list ['STnnnnn', 'ENTRY', 'A', 'C', 'DL8', 'E', 'F', 'H', 'V' or 'X'] which are asmscan specific or assembler-codes generated for data definitions. Lowercase versions are accepted. Changes to asmscan may allow some support.

Usage: 

```
asmscan input.asm output.asm output.nam
 Converts longnames to shortnames and creates a translation file.
 ```
### objscan

Takes an older ESD style object and replaces ESD records with XSD records, based on a mapping file. This produces a new object which can contain both short names and long names for correct linker name resolving.

```
Usage: objscan input.obj input.nam output.obj
 Takes a pre-built name file and converts ESDs to XSDs
```

### prelink

Combines a list of source objects into a single target object, using a directory of library objects for automatic name resolution. The library directory must contain a plain text file listing the object filenames within the library directory and in the order of resolution desired.
The JCC-Library supplies both the objects and plain text file. The library may be updated by adding new objects to both the directory and the text file.

Options to rename external indentifiers enable older linkers to build the resulting object into a LoadLibrary member. The options are: to replace all names with STnnnnnn where n is a unique 6 digit number, replace only the long-names (longer than 8 characters) or replace no names at all (just combining the required objects into a normal XSD object.)

Once processed, the entry point may become ST000000 instead of @crt0 (or @@CRT0) if the linker needs to be given that option. Also, both prelink and objscan reidentify the objects processed as AMODE=ANY, RMODE=ANY leaving the final decision up to the linker.

```
Usage: prelink -test=file -r|s|x libdir target.obj user1.obj...
 All parameters are position specific and only the flags are optional,
 -test=file appends debugging information to the JCC test file.
 -r renames all symbols to STnnnnnn to hide their meanings,
 -s performs the default function of only renaming longnames,
 -x performs no renames at all, and leaves XSD records as-is.
 libdir must contain the file liblst.txt listing all objects.
 Additionally, libdir may be specified as 'nolib' to exclude the library.
 user1.obj may be replaced with -file.txt which lists all the user objects.
 ```

 ## License/ User Agreement

 **From `help/help.htm`**

**JCC and the JCC-Library is not free software.**

JCC has been made available to allow mainframe users to compile other peoples free software which was supplied in source code form. This is designed to aid the development of free utilities for mainframes, running on real hardware or within emulators like Hercules. End-users who do not write C programs are not required to make any payments at all for this use of JCC.

Programmers who develop free software using JCC are encouraged to pay an annual US$50 fee to allow further development of the JCC compiler software. This is not a strict rule however as that would go against the first principle of aiding the development of free software for mainframes. Likewise, programmers who just develop for themselves are encouraged to make a contribution but this is not a strict rule.

To save effort for end-users, source code and the JCC compiled objects may be distributed together without paying the annual fee, and without changing the agreement terms already stated.

Programmers who develop software with JCC which is given away free, but without source code, must pay the annual US$50 fee for the year in which the object code is built. One-off developments in this case will not require continued annual payment, however subsequent build releases which extend past a 12 month period will require continued annual payments.

Programmers who develop software with JCC which is sold, must pay the annual fee of US$250 - all other conditions of the previous paragraph apply. Businesses which use JCC must pay the annual fee of US$250 regardless of what intentions they have for the software they develop with JCC.

As part of the conditions of use, you must agree not to sue me for any issue related to JCC or the JCC-Library product, and to take full responsibility for programs generated by JCC. JCC comes with no warranty, implied or otherwise. Neither JCC, nor the JCC-Library may be used to create or otherwise enhance another brand of C compiler. You hereby agree to delete JCC and the JCC library files if you do not agree to the other terms within this document.

Bug reports are always welcome, however only paying and current subscribers will be informed of updates to the code-generator/library when things are fixed or enhanced. This is due to my limited resources and to encourage people described in paragraph 2 to make a contribution. Having said all that, much effort has been spent testing the various components of JCC and it should produce correctly operating programs based on the ANSI C standard with only some small issues relating to the MVS file-system.

**What's free:**

The utilities which come with JCC (the scanners and prelinker) are supplied in source form and are free for anybody to enhance and use. I encourage this, and ask that useful modifications be sent back to myself so all mainframe users benefit. These programs have been enhanced to compile with JCC, for running on a mainframe - but no pre-built objects for these utilities have been supplied.

For Hercules and MVS3.8j, I supply an FTP Daemon to facilitate the easy uploading and downloading of source/object files when using the IFOX assembler. This version of FTP is designed for JCC (a previous release contained inline assembler which JCC doesn't support.)

I encourage end-users of FTP to contribute a one-off US$50 payment if they like and use FTP for MVS38j - but this is not part of the JCC license agreement.

**Who am I?**

Jason Paul Winter
jasonwinter hotmail com
(add an @ sign and a . to the address shown, and use a subject of: JCC)

JCC uses "parts" of the freely available lcc compiler front-end.