#ifndef IPC_SOCKET_H
#define IPC_SOCKET_H
/*#*************************************************************************
 ** Socket abstraction $Revision: 1.7 $
 ***************************************************************************
 ** (c) Konrad Rosenbaum, 2000
 ** protected by the GNU GPL version 2 or any newer
 ***************************************************************************
 ** History:
 **
 ** $Log: socket.h,v $
 ** Revision 1.7  2001/08/30 18:04:59  pandur
 ** *** empty log message ***
 **
 ** Revision 1.6  2000/11/26 12:06:03  pandur
 ** *added socket.cpp
 **
 ** Revision 1.5  2000/11/25 15:23:35  pandur
 ** orb added, some new code
 **
 ** Revision 1.4  2000/10/31 15:04:19  pandur
 ** more of few
 **
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

class QSocketNotifier;

/**
  @short Socket abstraction class
  @internal
  
  This is an internal class. Used to communicate via INET-connections. Every
  IConnection-instance may have several sockets.
*/
class ISocket:public QObject{
        Q_OBJECT
        
        public:
          enum ConMode {IPv4,IPv6,Local};
          enum Mode {Raw,Control,Data};
        
          /** attaches to an existing socket*/
          ISocket(int fd,Mode mode=Raw);
          /** attaches to an interface/port */
          ISocket(const char*ifc,uint32 port=0,ConMode cmode=IPv4,Mode mode=Raw);
          
          /**Closes and deconstructs the socket*/
          ~ISocket();
          
          /**reads size bytes from the socket, if exact ist true it returns
          data only if enough is available, elsewise it doesn't return anything*/
          ssize_t read(void*buf,size_t size,bool exact=true);
          /**true if at least size bytes are available*/
          bool readable(size_t size=1);
          /**true if the socket can receive data*/
          bool writeable();
          
          enum Side{None,Read,Write,Both};
          /**closes one or both halfes of the socket, according to which*/
          void halfclose(Side which=Write);
          /**equivalent to halfclose(Both), closes both sides of the socket*/
          void close();
          
          /**sets the maximum buffer for reading, default is 1k*/
          bool setReadBuffer(int);
          /**sets the maximum buffer for writing, default is 1k*/
          bool setWriteBuffer(int);
          
          /**returns the size of the read-buffer*/
          int readBuffer();
          /**returns the size of the write-buffer*/
          int writeBuffer();
          
          enum State{OK=0,LostConnection,NotConnected};
          State status(){return state;}
          
        public slots:
          void write(const void*buf,size_t size);
          
        private slots:
          /**internal slot for async mode*/
          void readable();
          /**internal slot for async mode*/
          void writeableslot();
          
        signals:
          /**thrown whenever errors occur*/
          void error(const QString&);
          /**async-read: throws out exactly one 128byte package*/
          void readPackage(void*);
          
        private:
          /**current state of the socket*/
          State state;
          /**Mode of operation (unused)*/
          Mode mode;
          /**system specific socket descriptor*/
          int sockfd;
          /**internal buffers*/
          void*rbuf,*wbuf;
          /**internal buffer sizes and positions*/
          int rbsz,wbsz,rbps,wbps;
          /**async write (yet not used)*/
          QSocketNotifier *wsn;
          /**for package reading mode*/
          QSocketNotifier *rsn;
          /**implemented for the case we use package mode*/
          void connectNotify(const char*);
          /**implemented for the case we use package mode*/
          void disconnectNotify(const char*);
          /**counts connections for package mode*/
          int pmodecnt;
};


#endif //IPC_SOCKET_H
