/*#*************************************************************************
 ** Socket abstraction $Revision: 1.2 $
 ***************************************************************************
 ** (c) Konrad Rosenbaum, 2000
 ** protected by the GNU GPL version 2 or any newer
 ***************************************************************************
 ** History:
 **
 ** $Log: socket.h,v $
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
          enum Mode {IPv4,IPv6,Local};
        
          /** attaches to an existing socket*/
          ISocket(int fd);
          /** attaches to an interface/port */
          ISocket(const char*ifc,uint32 port,Mode mode=IPv4);
        
          ~ISocket();
          
          ssize_t read(void*buf,size_t size,bool exact=true);
          bool readable();
          bool writeable();
          
          void halfclose();
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
