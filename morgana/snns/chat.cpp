/*#*********************************************************************
 ** Chat packages
 **
 ** (c) Konrad Rosenbaum, 2000
 ** protected by the GNU GPL v.2 or any newer
 ***********************************************************************
 ** $Log: chat.cpp,v $
 ** Revision 1.1  2000/09/21 18:40:56  pandur
 ** init
 **
 ***********************************************************************/


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

void SMain::reqChat(IPackage&p,IConnection*c)
{
        p.pos(0);
        QString s;
        p>>s;
        
        QStringList *sl=conFlags.find((void*)c);
        
        if(s=="listen"){
                if(sl==0){
                        sl=new QStringList("chatListen");
                        conFlags.insert((void*)c,sl);
                }else{
                        if(sl->findIndex("chatListen")<0)
                                *sl<<"chatListen";
                }
        }else
        if(s=="unlisten"){
                if(sl!=0)
                        sl->remove("chatListen");
        }else
        if(s=="send"){
                QObjectList*list=queryList("IConnection");
                QObjectListIt it(*list);
                QObject*tcon;
                while((tcon=it.current())!=0){
                        ++it;
                        QStringList*ssl=conACL.find((void*)tcon);
                        if(ssl->findIndex("chat")<0)continue;
                        ssl=conFlags.find((void*)tcon);
                        if(ssl==0)continue;
                        if(ssl->findIndex("chatListen")>=0){
                                ((IConnection*)tcon)->send(p);
                        }
                }
                delete list;
        }
#ifdef MDEBUG
        else mDebug("SNNS: unknown chat request (%s)\n",(const char*)s);
#endif
}

