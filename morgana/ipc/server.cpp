/*#***********************************************************************
 ** Server Class for IPC $Revision: 1.1 $
 *************************************************************************
 ** (c) Konrad Rosenbaum, 2000
 ** protected by the GNU GPL version 2 or any newer
 *************************************************************************
 ** $Log: server.cpp,v $
 ** Revision 1.1  2000/10/31 15:04:19  pandur
 ** more of few
 **
 *************************************************************************/

#include "server.h"
#include "debug.h"
#include <posix/socket.h>

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
                Error="can't create socket";
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
                        Error="can't resolve interface name";
                        return;
                }
                memcpy(&sa.sin_addr,*(he->h_addr_list),he->h_length);
        }else{//no interface given
                uint32 n=0;
                memcpy(&sa.sin_addr,&n,4);//any interface available
        }
        if(bind(fd,(sockaddr*)(&sa),sizeof(sa))){
                mDebug("IPC: couldn't bind socket\n");
                Error="couldn't bind socket";
                close(fd);fd=-1;
                return;
        }
        if(listen(fd,1)){
                mDebug("IPC: couldn't enter listen state\n");
                Error="couldn't enter listen state";
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


