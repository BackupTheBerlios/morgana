#ifndef IPC_CONNECTION_H
#define IPC_CONNECTION_H
/*#*************************************************************************
 ** Connection to peers $Revision: 1.6 $
 ***************************************************************************
 ** (c) Konrad Rosenbaum, 2000
 ** protected by the GNU GPL version 2 or any newer
 ***************************************************************************
 ** History:
 **
 ** $Log: connection.h,v $
 ** Revision 1.6  2001/08/30 18:04:59  pandur
 ** *** empty log message ***
 **
 ** Revision 1.5  2000/11/25 15:23:35  pandur
 ** orb added, some new code
 **
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
class IConnection:public QObject
{
        Q_OBJECT
        
        public:
          IConnection(ISocket*);
          ~IConnection();
        
	  bool rconnect(const QString&rsignal,QObject*lobj,char*lslot);
	  bool rconnect(QObject*lobj,char*lsignal,const QString&rslot);
	  
	  QStringList objectsin(QString);
          QStringList signalsof(QString);
          QStringList slotsof(QString);
        
        private:
          /**data socket*/
          ISocket *dsoc;
          /* *control socket ?needed?*/
          //ISocket *ctlsock;
          /* *temporary server socket*/
          //IServer *server;
};


#endif //IPC_CONNECTION_H
