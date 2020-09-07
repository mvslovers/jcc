/*

  Machine Limits C-Library Include File for MVS-OS/390-zOS

  (C)2003 Jason Paul Winter, All Rights Reserved.

*/

#ifndef _MACHINE_LIMITS_H_
#define _MACHINE_LIMITS_H_

#define CHAR_BIT  8          /* number of bits in a char */

#define SCHAR_MAX 127        /* min value for a signed char */
#define SCHAR_MIN (-128)     /* max value for a signed char */

#define UCHAR_MAX 255        /* max value for an unsigned char */
#define CHAR_MAX  255        /* max value for a char */
#define CHAR_MIN  0          /* min value for a char */

#define USHRT_MAX 65535      /* max value for an unsigned short */
#define SHRT_MAX  32767      /* max value for a short */
#define SHRT_MIN  (-32768)   /* min value for a short */

#define UINT_MAX  0xffffffff     /* max value for an unsigned int */
#define INT_MAX   0x7fffffff     /* max value for an int */
#define INT_MIN   ((-INT_MAX)-1) /* min value for an int */

#define ULONG_MAX 0xffffffff      /* max value for an unsigned long */
#define LONG_MAX  0x7fffffff      /* max value for a long */
#define LONG_MIN  ((-LONG_MAX)-1) /* min value for a long */

#define ULLONG_MAX 0xffffffffffffffffLL /* max value for an unsigned long long */
#define LLONG_MAX  0x7fffffffffffffffLL /* max value for a long long */
#define LLONG_MIN  ((-LLONG_MAX)-1)     /* min value for a long long */

#define PTRDIFF_MAX UINT_MAX  /* max value of a pointer */
#define PTRDIFF_MIN INT_MIN   /* min value of a pointer */

#define SSIZE_MAX INT_MAX     /* max value for a ssize_t */
#define SIZE_MAX  UINT_MAX    /* max value for a size_t */

#ifndef HUGE_VAL
 extern double     __infinity;
 #define HUGE_VAL (__infinity)
#endif

#endif /* !_MACHINE_LIMITS_H_ */
