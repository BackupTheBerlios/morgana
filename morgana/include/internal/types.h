#ifndef M_TYPES_H
#define M_TYPES_H

#include "../config.h"

#ifdef WORDS_BIGENDIAN
# define _SWAB2(x) (x)
# define _SWAB4(x) (x)
# define _SWAB8(x) (x)
#else //LITLLEENDIAN
# define _SWAB2(x) ( (((x)&0xff)<<8) | (((x)&0xff00)>>8) )
# define _SWAB4(x) ( (((x)&0xff)<<24) | (((x)&0xff00)<<8) | \
        	     (((x)>>8)&0xff00) | (((x)>>24)&0xff) )
# if (SIZEOF_INT == 8) || (SIZEOF_LONG_LONG == 8)
#  define _SWAB8(x) ( (((x)&0xff)<<56) | (((x)&0xff00)<<40) | \
                      (((x)&0xff0000)<<24) | (((x)&0xff000000)<<8) | \
                      (((x)>>8)&0xff000000) | (((x)>>24)&0xff0000) | \
                      (((x)>>40)&0xff00) | (((x)>>56)&0xff) )
# else //no int64
#  define _SWAB8(x) (x.swab())
# endif //int64
#endif //ENDIANESS


#define SWAB8(x) (x)
#define SWAB16(x) _SWAB2(x)
#define SWAB32(x) _SWAB4(x)
#define SWAB64(x) _SWAB8(x)                                          

#ifndef NULL
#define NULL 0
#endif

#if SIZEOF_BOOL == 0
typedef enum {false=0,true=1} bool;
#endif //BOOL

#if SIZEOF_CHAR == 1 //int8
typedef char int8;
typedef signed char sint8;
typedef unsigned char uint8;
#elif SIZEOF_SHORT == 1
typedef short int8;
typedef signed short sint8;
typedef unsigned short uint8;
#else
# error No 8-bit type found
#endif //int8

//int16
#if SIZEOF_SHORT == 2
typedef short int16;
typedef signed short sint16;
typedef unsigned short uint16;
#elif SIZEOF_INT == 2
typedef int int16;
typedef signed int sint16;
typedef unsigned int uint16;
#else
# error No 16-bit int type found
#endif

//int32
#if SIZEOF_LONG == 4
typedef long int32;
typedef signed long sint32;
typedef unsigned long uint32;
#elif SIZEOF_INT == 4
typedef int int32;
typedef signed int sint32;
typedef unsigned int uint32;
#elif SIZEOF_SHORT == 4
typedef short int32;
typedef signed short sint32;
typedef unsigned short uint32;
#elif SIZEOF_LONG_LONG ==4
typedef long long int32;
typedef signed long long sint32;
typedef unsigned long long uint32;
#else
# error No 32-bit type found
#endif

//int64
#define HAVE_INT64 1
#if SIZEOF_LONG == 8
typedef long int64;
typedef signed long sint64;
typedef unsigned long uint64;
#elif SIZEOF_LONG_LONG == 8
typedef long long int64;
typedef signed long long sint64;
typedef unsigned long long uint64;
#elif SIZEOF_INT == 8
typedef int int64;
typedef signed int sint64;
typedef unsigned int uint64;
#else
#include "int64.h"
#undef HAVE_INT64
#endif

#endif //M_TYPES_H
