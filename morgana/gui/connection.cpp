/*#*********************************************************************
 ** IPC-Connection Frontend
 **
 ** (c) Konrad Rosenbaum, 2000
 ** protected by the GNU GPL v.2 or any newer
 ***********************************************************************
 ** $Log: connection.cpp,v $
 ** Revision 1.1  2000/09/21 18:40:52  pandur
 ** init
 **
 ***********************************************************************/


#include <ipc/connection.h>


#include "connection.h"


MConnect::MConnect(QWidget * parent, const char * name, bool modal, WFlags f)
 :QTabDialog(parent,name,modal,f)
{
        
}

MConnect::~MConnect()
{
}

bool MConnect::haveConnection()
{
        return false;
}

IConnection*MConnect::connection()
{
        return 0;
}

void MConnect::found(const IClient::Address&){}
