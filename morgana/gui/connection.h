/*#*********************************************************************
 ** IPC-Connection Frontend
 **
 ** (c) Konrad Rosenbaum, 2000
 ** protected by the GNU GPL v.2 or any newer
 ***********************************************************************
 ** $Log: connection.h,v $
 ** Revision 1.1  2000/09/21 18:40:52  pandur
 ** init
 **
 ***********************************************************************/
#ifndef M_CONNECTION_H
#define M_CONNECTION_H

#include <qtabdialog.h>

class IConnection;
class QLineEdit;
#include <ipc/client.h>

/**
  @short Dialog to connect to an existing server
*/
class MConnect:public QTabDialog
{
        Q_OBJECT
        public:
          MConnect(QWidget * parent=0, const char * name=0, bool modal=false, WFlags f=0 );
          ~MConnect();
          
          bool haveConnection();
          IConnection*connection();
        
        private slots:
          void found(const IClient::Address&);
          
        private:
          QLineEdit*edt;
          IConnection*con;
};



#endif //M_CONNECTION_H
