//-< STDTP.H >-------------------------------------------------------*--------*
// SAL                       Version 1.0         (c) 1997  GARRET    *     ?  *
// (System Abstraction Layer)                                        *   /\|  *
//                                                                   *  /  \  *
//                          Created:      7-Jan-97    K.A. Knizhnik  * / [] \ *
//                          Last update: 23-Apr-97    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Standart type and macro definitions
//-------------------------------------------------------------------*--------*

#ifndef __STDTP_H__
#define __STDTP_H__

#define PTHREADS 1

#if defined(_WIN32) && !defined(__CYGWIN__)
#include <windows.h>
#pragma warning(disable:4800 4355 4146)
#endif

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <stdarg.h>

//
// Do not used builtin C++ bool type because as far as sizeof(bool) == 1
// concurrent access to neighboring boolean componets by different threads
// will require locking
//
#define boolean  int
#ifndef True
#define True     1
#endif
#ifndef False
#define False    0
#endif

// Align value 'x' to boundary 'b' which should be power of 2
#define DOALIGN(x,b)   (((x) + (b) - 1) & ~((b) - 1))

#define local static

typedef signed char    int1;
typedef unsigned char  nat1;

typedef signed short   int2;
typedef unsigned short nat2;

typedef signed int     int4;
typedef unsigned int   nat4;

#if defined(__GNUC__)
typedef unsigned long long nat8;
typedef signed   long long int8;
#define INT8_FORMAT "ll"
#define CONST64(c)  c##LL
#else
#if defined(_WIN32)
typedef unsigned __int64 nat8;
typedef __int64          int8;
#define INT8_FORMAT "I64"
#define CONST64(c)  c
#else
#if defined(__osf__ )
typedef unsigned long nat8;
typedef signed   long int8;
#define INT8_FORMAT "l"
#define CONST64(c)  c##L
#else
#error "integer 8 byte type is not defined"
#endif
#endif
#endif

#define nat8_low_part(x)  ((nat4)(x))
#define nat8_high_part(x) ((nat4)((nat8)(x)>>32))
#define int8_low_part(x)  ((int4)(x))
#define int8_high_part(x) ((int4)((int8)(x)>>32))
#define cons_nat8(hi, lo) ((((nat8)(hi)) << 32) | (nat4)(lo))
#define cons_int8(hi, lo) ((((int8)(hi)) << 32) | (nat4)(lo))

#define MAX_NAT8  nat8(-1)

typedef float  real4;
typedef double real8;

#ifndef BIG_ENDIAN
#define BIG_ENDIAN      4321    /* most-significant byte first (IBM, net) */
#endif
#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN   1234
#endif

#ifndef BYTE_ORDER
#if defined(__sparc__) || defined(__m68k__)
#define BYTE_ORDER      BIG_ENDIAN
#else
#define BYTE_ORDER      LITTLE_ENDIAN
#endif
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
typedef HANDLE descriptor_t;
#else
typedef int descriptor_t;
#endif

#define   items(array) (sizeof(array)/sizeof*(array))

#endif




