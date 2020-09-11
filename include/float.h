/*

  Float C-Library Include File for MVS-OS/390-zOS

  (C)2003 Jason Paul Winter, All Rights Reserved.

*/

#ifndef __FLOATH
#define __FLOATH /* Prevent multiple includes */

#define FLT_RADIX        16
#define FLT_ROUNDS       0

#define FLT_DIG          6
#define FLT_EPSILON      9.536743e-7F
#define FLT_MANT_DIG     6
#define FLT_MIN          5.397606e-79F
#define FLT_MIN_EXP      (-64)
#define FLT_MIN_10_EXP   (-78)
#define FLT_MAX          7.237005e+75F
#define FLT_MAX_EXP      63
#define FLT_MAX_10_EXP   75

#define DBL_DIG          15
#define DBL_EPSILON      2.2204460492503131e-16
#define DBL_MANT_DIG     14
#define DBL_MIN          5.3976053469340279e-79
#define DBL_MIN_EXP      (-64)
#define DBL_MIN_10_EXP   (-78)
#define DBL_MAX          7.2370055773322621e+75
#define DBL_MAX_EXP      63
#define DBL_MAX_10_EXP   75

#define LDBL_DIG         DBL_DIG
#define LDBL_EPSILON     DBL_EPSILON
#define LDBL_MANT_DIG    DBL_MANT_DIG
#define LDBL_MIN         DBL_MIN
#define LDBL_MIN_EXP     DBL_MIN_EXP
#define LDBL_MIN_10_EXP  DBL_MIN_10_EXP
#define LDBL_MAX         DBL_MAX
#define LDBL_MAX_EXP     DBL_MAX_EXP
#define LDBL_MAX_10_EXP  DBL_MAX_10_EXP

#endif
