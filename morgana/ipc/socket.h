#ifndef IPC_SOCKET_H
#define IPC_SOCKET_H
/*#*************************************************************************
 ** Socket abstraction $Revision: 1.3 $
 ***************************************************************************
 ** (c) Konrad Rosenbaum, 2000
 ** protected by the GNU GPL version 2 or any newer
 ***************************************************************************
 ** History:
 **
 ** $Log: socket.h,v $
 ** Revision 1.3  2000/10/29 14:37:10  pandur
 ** handler moves into connection <-> everything is an object
 **
 ** Revision 1.2  2000/09/24 16:22:24  pandur
 ** *** empty log message ***
 **
 ** Revision 1.1  2000/09/21 18:40:56  pandur
 ** init
 **
 **
 ***************************************************************************/

#include <posix/file.h>
#include <qobject.h>
#include <internal/types.h>

/**
  @short Socket abstraction class
*/
class ISocket:public QObject{
        Q_OBJECT
        
        public:
          enum ConMode {IPv4,IPv6,Local};
          enum Mode {Control,Data};
        
          /** attaches to an existing socket*/
          ISocket(int fd,Mode mode=Control);
          /** attaches to an interface/port */
          ISocket(const char*ifc,uint32 port=0,ConMode cmode=IPv4,Mode mode=Control);
        
          ~ISocket();
          
          ssize_t read(void*buf,size_t size,bool exact=true);
          bool readable();
          bool writeable();
          
          enum Side{None,Read,Write,Both};
          void halfclose(Side which=Write);
          void close();
          
          bool setReadBuffer(int);
          bool setWriteBuffer(int);
          
          int readBuffer();
          int writeBuffer();
          
          enum State{OK=0,LostConnection,NotConnected};
          State status(){return state;}
          
        public slots:
          void write(const void*buf,size_t size);
          
        signals:
          void error(const QString&);
          
        private:
          State state;
          int sockfd;
};


#endif //IPC_SOCKET_H
