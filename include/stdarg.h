/*

  StdArg C-Library Include File for MVS-OS/390-zOS

  (C)2003 Jason Paul Winter, All Rights Reserved.

*/

#ifndef __STDARGH
#define __STDARGH /* Prevent multiple includes */

typedef char * va_list;

#define __va_size(type) ((sizeof (type) + sizeof (char *) - 1) & ~(sizeof (char *) - 1))

#define va_start(ap, last) (ap) = ((va_list)&(last) + __va_size(last))

#define va_copy(dest, src) (dest) = (va_list)(src)

#define va_arg(ap, type) (*(type *)(((ap) += __va_size(type)) - sizeof(type)))

#define va_end(ap) (ap) = (va_list)0

#endif
