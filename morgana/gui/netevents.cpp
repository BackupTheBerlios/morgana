/*#*********************************************************************
 ** Events distributed through neural networks
 **
 ** (c) Konrad Rosenbaum, 2000
 ** protected by the GNU GPL v.2 or any newer
 ***********************************************************************
 ** $Log: netevents.cpp,v $
 ** Revision 1.1  2000/09/21 18:40:52  pandur
 ** init
 **
 ***********************************************************************/

#if __GNUC__ == 2
#pragma implementation "netevents.h"
#pragma implementation "unit.h"
#endif

#include "netevents.h"
#include "../snns/unit.h"
#include <ipc/package.h>
#include <ipc/connection.h>

MEventList::MEventList(IPackage&p)
{
        list.setAutoDelete(true);
        p.pos(0);
        QString n=p.name(),c;p>>c;
        //Unit Events
        if((n=="get"&&c=="unit")||
           (n=="set"&&(c=="create unit"||c=="modify unit"))){
                //unit manipulation events
                //load nums
                uint32 net;uint16 nou;
                p>>net>>nou;
                SUnit u;
                //load units
                for(;nou>0;nou--){
                        u.mask=0;
                        getUnit(u,p);
                        list.append(new MUnitEvent(net,u));
                }
        }else
        if(n=="set"&&c=="delete unit"){
                //unit deletion events
                uint32 net,unt;uint16 nou;
                p>>net>>nou;
                for(;nou>0;nou--){
                        p>>unt;
                        list.append(new MUnitEvent(net,unt,true));
                }
        }
        //Connection Events
        //SNNS Events
}

void MEventList::sendout(IConnection*c)const
{
        //sort events
        QListIterator<MBaseEvent>it(list);
        MBaseEvent *ev;
        QList<MUnitEvent>ulst,dulst;
        QList<MConnectionEvent>clst,dclst;
        QList<MNetworkEvent>nlst,dnlst;
        while((ev=it.current())!=0){
                ++it;
                switch(ev->type()){
                        case MBaseEvent::Unit:{
                                if(ev->isDelete())dulst.append((MUnitEvent*)ev);
                                else ulst.append((MUnitEvent*)ev);
                                break;
                        }
                        case MBaseEvent::Connection:{
                                if(ev->isDelete())dclst.append((MConnectionEvent*)ev);
                                else clst.append((MConnectionEvent*)ev);
                                break;
                        }
                        case MBaseEvent::Network:{
                                if(ev->isDelete())dnlst.append((MNetworkEvent*)ev);
                                else nlst.append((MNetworkEvent*)ev);
                                break;
                        }
                        default: break; //ignore
                }
        }
        //send out unit events
        if(!ulst.isEmpty()){
                uint16 nou=ulst.count();
                uint32 net;
                IPackage p("set");
                QListIterator<MUnitEvent>it(ulst);
                it.current()->getNet(net);
                p<<(QString)"modif unit"<<net<<nou;
                SUnit u;
                MUnitEvent *ev;
                while((ev=it.current())!=0){
                        ++it;
                        ev->toUnit(u);
                        setUnit(u,p);
                }
                c->send(p);
        }
        //send out conn-events
        
        //send out net-events
        
        //send out unit-delete-events
        if(!dulst.isEmpty()){
                uint16 nou=dulst.count();
                IPackage p("set");
                uint32 nid;
                QListIterator<MUnitEvent>it(ulst);
                it.current()->getNet(nid);
                p<<(QString)"delete unit"<<nid<<nou;
                MUnitEvent *ev;
                while((ev=it.current())!=0){
                        ++it;
                        ev->getId(nid);
                        p<<nid;
                }
        }
}


MUnitEvent::MUnitEvent(uint32 n,const SUnit&u)
{
        setNet(n);
        if(u.mask&UPROTO)setProto(u.prototype);
        if(u.mask&UPOS)setPos(u.xpos,u.ypos,u.zpos);
        if(u.mask&UBIAS)setBias(u.bias);
        if(u.mask&UACT)setAct(u.act);
        if(u.mask&UACTF)setActFunc(u.actfunc);
        if(u.mask&UOUTF)setOutFunc(u.outfunc);
        if(u.mask&UNAME)setName(u.name);
        if(u.mask&UNUM)setId(u.id);
        if(u.mask&UERR)setError(u.errcode);
}

void MUnitEvent::toUnit(SUnit&u)const
{
        u.mask=0;
        if(hasProto()){u.mask|=UPROTO;getProto(u.prototype);}
        if(hasPos()){u.mask|=UPOS;getPos(u.xpos,u.ypos,u.zpos);}
        if(hasBias()){u.mask|=UBIAS;getBias(u.bias);}
        if(hasAct()){u.mask|=UACT;getAct(u.act);}
        if(hasActFunc()){u.mask|=UACTF;getActFunc(u.actfunc);}
        if(hasOutFunc()){u.mask|=UOUTF;getOutFunc(u.outfunc);}
        if(hasName()){u.mask|=UNAME;getName(u.name);}
        if(hasId()){u.mask|=UNUM;getId(u.id);}
}

MUnitEvent* MEventList::findUnit(const MUnitID&id)
{
        QListIterator<MBaseEvent> it(list);
        MBaseEvent*ev;
        for(;(ev=it.current())!=0;++it){
                if(ev->type()==MBaseEvent::Unit){
                        if(*((MUnitEvent*)ev) == id)
                                return (MUnitEvent*)ev;
                }
        }
        return 0;
}

MConnectionEvent* MEventList::findConnection(const MConnectionID&id)
{
        QListIterator<MBaseEvent> it(list);
        MBaseEvent*ev;
        for(;(ev=it.current())!=0;++it){
                if(ev->type()==MBaseEvent::Connection){
                        if(*((MConnectionEvent*)ev)==id)
                                return (MConnectionEvent*)ev;
                }
        }
        return 0;
}

MNetworkEvent* MEventList::findNetwork(uint32 id)
{
        QListIterator<MBaseEvent> it(list);
        MBaseEvent*ev;
        for(;(ev=it.current())!=0;++it){
                if(ev->type()==MBaseEvent::Network){
                        if(*((MNetworkEvent*)ev)==id)
                                return (MNetworkEvent*)ev;
                }
        }
        return 0;
}

