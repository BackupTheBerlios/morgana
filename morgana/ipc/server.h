#ifndef IPC_SERVER_H
#define IPC_SERVER_H

/*#***********************************************************************
 ** Server Class for IPC $Revision: 1.3 $
 *************************************************************************
 ** (c) Konrad Rosenbaum, 2000
 ** protected by the GNU GPL version 2 or any newer
 *************************************************************************
 ** $Log: server.h,v $
 ** Revision 1.3  2000/11/05 07:21:11  pandur
 ** implemented server socket
 **
 ** Revision 1.2  2000/10/31 15:04:19  pandur
 ** more of few
 **
 ** Revision 1.1  2000/10/29 14:39:19  pandur
 ** *** empty log message ***
 **
 *************************************************************************/

#include <qobject.h>
#include <internal/types.h>
#include <qstring.h>

class IConnection;
class QSocketNotifier;

/**
  @short Server Socket abstraction
  
  
*/
class IServer:public QObject{
        Q_OBJECT
        
        public:
          enum Mode {IPv4,IPv6,Local};
        
          /**open server socket, currently only IPv4 works. You should
          call error() after creating a server socket to determine whether
          it succeeded.*/
          IServer(const char*ifc,uint32 port=0,Mode mode=IPv4);
          ~IServer();
          
          /**returns interface in decimal-dot-notation, returns NULL
          on error.
          
          If you need the result for longer: copy it, the call will 
          overwrite it*/
          const char*interface();
          
          /**Tries to resolve interface ip, returns NULL on error.*/
          const char*interfaceName();
          /**returns the port number which was assigned to this socket*/
          uint32 port();
          
          /**returns the last error which occurred and resets it*/
          int error(){int i=ierrno;ierrno=0;return i;}
          /**returns the last error which occurred and resets it*/
          QString strerror(){return ::strerror(error());}
          
        private slots:
          /**
          @internal
          called whenever there is some connection to accept*/
          void accept();
          
        signals:
          /**Emitted whenever a new connection was accepted.*/
          void newConnection(IConnection*);
          /**Emitted whenever an error occurs.*/
          void error(int);
          /**Emitted whener a fatal error occurs, you should terminate the
          server-socket-object after receiving this signal.*/
          void fatalError(int);
          
        private:
          int fd;
          int ierrno;
          QSocketNotifier*sn;
};

#endif //IPC_SERVER_H
