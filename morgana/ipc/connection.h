/*#*************************************************************************
 ** Connection to peers $Revision: 1.4 $
 ***************************************************************************
 ** (c) Konrad Rosenbaum, 2000
 ** protected by the GNU GPL version 2 or any newer
 ***************************************************************************
 ** History:
 **
 ** $Log: connection.h,v $
 ** Revision 1.4  2000/11/05 07:21:11  pandur
 ** implemented server socket
 **
 ** Revision 1.3  2000/10/31 15:04:19  pandur
 ** more of few
 **
 ** Revision 1.2  2000/10/29 14:37:10  pandur
 ** handler moves into connection <-> everything is an object
 **
 ** Revision 1.1  2000/09/24 16:22:24  pandur
 ** *** empty log message ***
 **
 **
 ***************************************************************************/

#include <qobject.h>

class ISocket;
class IServer;

/**
  @short Connection to peers
  
  This class implements virtual connections to other processes. It is the
  interface seen by the application.
*/
class IConnection:public QObject{
        Q_OBJECT
        
        public:
          IConnection();
          /**called by IServer*/
          IConnection(int);
          ~IConnection();
        
        
        private:
          /**data socket*/
          ISocket *dsoc;
          /**control socket*/
          ISocket *ctlsock;
          /**temporary server socket*/
          IServer *server;
};
