/*#*************************************************************************
 ** Connection to peers $Revision: 1.1 $
 ***************************************************************************
 ** (c) Konrad Rosenbaum, 2000
 ** protected by the GNU GPL version 2 or any newer
 ***************************************************************************
 ** History:
 **
 ** $Log: connection.h,v $
 ** Revision 1.1  2000/09/24 16:22:24  pandur
 ** *** empty log message ***
 **
 **
 ***************************************************************************/

#include <qobject.h>

class ISocket;
class IHandler;

/**
  @short Connection to peers
  
  This class implements virtual connections to other processes.
*/
class IConnection:public QObject{
        Q_OBJECT
        
        public:
          IConnection();
          ~IConnection();
        
        
        private:
          QList<ISocket>socks;
          QList<IHandler>handler;
};
