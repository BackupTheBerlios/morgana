#ifndef M_DEBUG_H
#define M_DEBUG_H

#ifdef MDEBUG
#include <stdio.h>
/**
 @internal
*/
extern "C" int getpid();
#define mDebug(format, args...) fprintf(stderr,"%i-" format,getpid() , ## args)

#else //!MDEBUG

#define mDebug(format, args...)

#endif //MDEBUG

#endif //M_DEBUG_H
