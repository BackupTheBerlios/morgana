#ifndef IPC_SERVER_H
#define IPC_SERVER_H

/*#***********************************************************************
 ** Server Class for IPC $Revision: 1.1 $
 *************************************************************************
 ** (c) Konrad Rosenbaum, 2000
 ** protected by the GNU GPL version 2 or any newer
 *************************************************************************
 ** $Log: server.h,v $
 ** Revision 1.1  2000/10/29 14:39:19  pandur
 ** *** empty log message ***
 **
 *************************************************************************/

#include <qobject.h>
#include <internal/types.h>

class IConnection;
class QSocketNotifier;
class QString;

/**
  @short Server Socket abstraction
  
  
*/
class IServer:public QObject{
        Q_OBJECT
        
        public:
          enum Mode {IPv4,IPv6,Local};
        
          IServer(const char*ifc,uint32 port=0,Mode mode=IPv4);
          ~IServer();
          
          const char*interface();
          const char*interfaceName();
          uint32 port();
          
        signals:
          void newConnection(IConnection*);
          void error(const QString&);
          void fatalError(const QString&);
          
        private:
          int fd;
          QSocketNotifier*sn;
};

#endif //IPC_SERVER_H
