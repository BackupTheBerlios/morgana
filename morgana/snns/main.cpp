/*#***********************************************************************
 ** Main functionality of the server class
 *************************************************************************
 ** $Log: main.cpp,v $
 ** Revision 1.1  2000/09/21 18:40:56  pandur
 ** init
 **
 **
 *************************************************************************/
 
//own
#include "main.h"

//Qt
#include <qapplication.h>
#include <qstring.h>
#include <qobjcoll.h>

//IPC
#include <ipc/listener.h>
#include <ipc/package.h>
#include <ipc/connection.h>

////////////////////
//Debug-section
#include "debug.h"
///////////////////

//needed for communication with parent
#include <iostream.h>

SMain::SMain()
{
        conACL.setAutoDelete(true);
        conFlags.setAutoDelete(true);
        IListener *listener=new IListener("NN Server Alpha",
                          IListener::IPv4ANYADDR,IListener::IPv4ANYPORT);
        insertChild(listener);
        
        int e;
        if((e=listener->error())){
                mDebug("SNNS: got no listener (%X)\n",e);
                delete listener;
                cout<<-1<<flush;
                delete this;
                qApp->exit(1);
                return;
        }
        
        cout<<listener->port()<<flush;  
        mDebug("SNNS: using port %hi\n",listener->port());
        connect(listener,SIGNAL(newConnection(IConnection*)),
                this,SLOT(newConnection(IConnection*)));
}

SMain::~SMain()
{
        //Send disconnect signals
        IPackage p("SIGNAL");
        p<<(QString)"disconnect";
        QObjectList *list=queryList("IConnection");
        QObjectListIt it(*list);
        QObject*obj;
        while((obj=it.current())!=0){
                ++it;
                ((IConnection*)obj)->send(p);
        }
        delete list;
        //Listeners and connections are deleted by QObject::~QObject()
        //exit process
        qApp->exit();
}

void SMain::request(IPackage&p,IConnection*c)
{
        //debugging
        mDebug("SNNS: Package: %s(%li:%li)\n",(const char*)p.name(),p.length(),p.pos());
        //check acl
        QStringList *sl=conACL.find((void*)c);
        if(sl==0){
                mDebug("SNNS: internal error, couldn't find connection\n");
                return;
        }
        QString name=p.name();
        if(sl->findIndex(name)<0)return;
        //find package handler
        if(name=="request")reqRequest(p,c);else
        if(name=="set")reqSet(p,c);else
        
        if(name=="login")reqLogin(p,c);else
        if(name=="SIGNAL")reqSignal(p,c);else
        if(name=="chat")reqChat(p,c);
#ifdef MDEBUG
        else if(name=="ping"){
                mDebug("SNNS: ping received\n");
                c->send(p);
        }
        else mDebug("SNNS: unknown package %s\n",(const char*)name);
#endif
}

void SMain::newConnection(IConnection*con)
{
        mDebug("SNNS: new Connection on %s:%hi\n",(const char*)con->sockName(),con->sockPort());
        //connect for communication
        connect(con,SIGNAL(receive(IPackage&,IConnection*)),
                this,SLOT(request(IPackage&,IConnection*)));
        connect(con,SIGNAL(fatalError(IConnection*)),
                this,SLOT(termConnection(IConnection*)));
        //insert into internal lists
        insertChild(con);
        //create ACL
        QStringList *sl=new QStringList;
        *sl<<(QString)"request"<<(QString)"login";
        conACL.insert((void*)con,sl);
        //security: restrict package size:
        con->setMaxPackage(4096);
}

void SMain::termConnection(IConnection*con)
{
        IPackage p("SIGNAL");
        p<<(QString)"disconnect";
        con->send(p);
        conACL.remove(con);
        conFlags.remove(con);
        delete con;
}


///////////////////////////////////////////////////////////////////
// requests

void SMain::reqSignal(IPackage&p,IConnection*c)
{
        QString s;
        p>>s;
        if(s=="terminate"){
                delete c;
                delete this;
                mDebug("SNNS: terminating\n");
                return;
        }
        mDebug("SNNS: unknown signal %s\n",(const char*)s);
}

void SMain::reqRequest(IPackage&p,IConnection*c)
{
        
}

void SMain::reqLogin(IPackage&p,IConnection*c)
{
        //dummy:
        QStringList *sl=conACL.find((void*)c);
        *sl<<"ping"<<"SIGNAL"<<"admin"<<"get"<<"set"<<"eval"<<"learn"
           <<"listen"<<"chat";
        IPackage ap("login");
        ap<<true<<QString("logged in");
        c->send(ap);
        //unrestrict package size and increase buffers:
        c->setMaxPackage(0);
        c->setInputBufferSize(131072);//128kB
        c->setOutputBufferSize(131072);
}


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

/** initializes and starts IPC and the server*/
int main(int argc,char**argv)
{
        mDebug("SNNS: starting server\n");
        QApplication a(argc,argv,false);
        mDebug("SNNS: initializing\n");
        IConnection::setIgnoreSigPipe(true);
        new SMain;
        mDebug("SNNS: entering loop\n");
        int r=a.exec();
        mDebug("SNNS: exiting program (%i)\n",r);
        return r;
}
