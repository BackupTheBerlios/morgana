/*#***********************************************************************
 ** Server Class for IPC $Revision: 1.3 $
 *************************************************************************
 ** (c) Konrad Rosenbaum, 2000
 ** protected by the GNU GPL version 2 or any newer
 *************************************************************************
 ** $Log: server.cpp,v $
 ** Revision 1.3  2001/08/30 18:04:59  pandur
 ** *** empty log message ***
 **
 ** Revision 1.2  2000/11/05 07:21:11  pandur
 ** implemented server socket
 **
 ** Revision 1.1  2000/10/31 15:04:19  pandur
 ** more of few
 **
 *************************************************************************/

#include "server.h"
#include "debug.h"
#include "connection.h"
#include "socket.h"
#include <posix/socket.h>
#include <qsocketnotifier.h>
#include <stdio.h>

IServer::IServer(const char*ifc,uint32 port,Mode mode)
{
        //TODO: support for other protos than IP version 4
        if(mode!=IPv4){
                mDebug("IPC: warning: only IPv4 supported yet.\n");
        }
        
        //get a socket
        fd=socket(PF_INET,SOCK_STREAM,0);
        if(fd<0){
                mDebug("IPC: can't create socket\n");
                ierrno=errno;
                return;
        }
        //bind it...
        struct sockaddr_in sa;
        sa.sin_family=AF_INET;
        sa.sin_port=htons(port);
        if(ifc!=0){
                //try to resolve iface
                struct hostent *he=gethostbyname(ifc);
                if(!he){
                        mDebug("IPC: can't resolve interface name %s.\n",ifc);
                        ierrno=h_errno;
                        return;
                }
                memcpy(&sa.sin_addr,*(he->h_addr_list),he->h_length);
        }else{//no interface given
                uint32 n=0;
                memcpy(&sa.sin_addr,&n,4);//any interface available
        }
        if(bind(fd,(sockaddr*)(&sa),sizeof(sa))){
                mDebug("IPC: couldn't bind socket\n");
                ierrno=errno;
                close(fd);fd=-1;
                return;
        }
        if(listen(fd,1)){
                mDebug("IPC: couldn't enter listen state\n");
                ierrno=errno;
                close(fd);fd=-1;
        }
        sn=new QSocketNotifier(fd,QSocketNotifier::Read);
        connect(sn,SIGNAL(activated()),this,SLOT(accept()));
}

IServer::~IServer()
{
        if(sn)delete sn;
        if(fd>=0)close(fd);
}


const char*IServer::interface()
{
        struct sockaddr_in sa;
        socklen_t sl=sizeof(sa);
        if(getsockname(fd,(sockaddr*)&sa,&sl)<0){
                mDebug("IPC: error in retrieving socket addr: %s\n",strerror(errno));
                ierrno=errno;
                error(ierrno);
                return 0;
        }
        uint32 ip;
        memcpy(&ip,&sa.sin_addr,4);
        ip=ntohl(ip);
        static char s[17];
        sprintf(s,"%li.%li.%li.%li",(ip>>24)&0xff,(ip>>16)&0xff,(ip>>8)&0xff,ip&0xff);
        return s;
}

const char*IServer::interfaceName()
{
        struct sockaddr_in sa;
        socklen_t sl=sizeof(sa);
        if(getsockname(fd,(sockaddr*)&sa,&sl)<0){
                mDebug("IPC: error in retrieving socket addr: %s\n",strerror(errno));
                ierrno=errno;
                error(ierrno);
                return 0;
        }
        struct hostent *he=gethostbyaddr((char*)&sa.sin_addr,
                			 sizeof(sa.sin_addr),AF_INET);
        if(!he){
                mDebug("IPC: error in resolving socket addr: %s\n",strerror(errno));
                ierrno=h_errno;
                error(ierrno);
                return 0;
        }
        return he->h_name;
}

uint32 IServer::port()
{
        struct sockaddr_in sa;
        socklen_t sl=sizeof(sa);
        if(getsockname(fd,(sockaddr*)&sa,&sl)<0){
                mDebug("IPC: error in retrieving socket addr: %s\n",strerror(errno));
                ierrno=errno;
                error(ierrno);
                return 0;
        }
        return ntohs(sa.sin_port);
}


void IServer::accept()
{
        int nfd=::accept(fd,0,0);
        if(nfd>=0)newConnection(new IConnection(new ISocket(nfd)));
        else {
                ierrno=errno;
                error(ierrno);
        }
}

