/*#*************************************************************************
 ** Socket abstraction $Revision: 1.1 $
 ***************************************************************************
 ** (c) Konrad Rosenbaum, 2000
 ** protected by the GNU GPL version 2 or any newer
 ***************************************************************************
 ** History:
 **
 ** $Log: socket.cpp,v $
 ** Revision 1.1  2000/11/26 12:06:03  pandur
 ** *added socket.cpp
 **
 ***************************************************************************/

#include "socket.h"

#include <posix/memory.h>

ISocket::ISocket(int fd,Mode md)
{
        rbuf=malloc(1024);
        wbuf=malloc(1024);
        rbsz=wbsz=1024;
        state=OK;
        sockfd=fd;
        mode=md;
}

ISocket::ISocket(const char*ifc,uint32 port,ConMode cmode,Mode md)
{
        //resolve interface:
        
        //connect
        
        //establish
        rbuf=malloc(1024);
        wbuf=malloc(1024);
        rbsz=wbsz=1024;
        state=OK;
        mode=md;
}

ISocket::~ISocket()
{
        halfclose(Both);
        free(rbuf);
        free(wbuf);
}


