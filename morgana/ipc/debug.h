/*#*********************************************************************
 ** Debugger Macros
 **
 ** (c) Konrad Rosenbaum, 2000
 ** protected by the GNU GPL v.2 or any newer
 ***********************************************************************
 ** $Log: debug.h,v $
 ** Revision 1.1  2000/10/31 15:04:19  pandur
 ** more of few
 **
 ***********************************************************************/

#ifndef M_DEBUG_H
#define M_DEBUG_H

#ifdef MDEBUG
#include <stdio.h>
extern "C" int getpid();
#define mDebug(format, args...) fprintf(stderr,"%i-" format,getpid() , ## args)

#else //!MDEBUG

#define mDebug(format, args...)

#endif //MDEBUG

#endif //M_DEBUG_H
