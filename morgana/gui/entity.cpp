/***********************************************************************
 ** 2D Display Entities
 **
 ** (c) Konrad Rosenbaum, 2000
 ** protected by the GNU GPL v.2 or any newer
 ***********************************************************************
 ** $Log: entity.cpp,v $
 ** Revision 1.1  2000/09/21 18:40:52  pandur
 ** init
 **
 ***********************************************************************/

#include "entity.h"
#include "display.h"

#include <qobjectlist.h>
#include <qpainter.h>
#include <qwidget.h>

#include <math.h>

#include "debug.h"


//static data:

QPoint MNeuron::moveP;
static MNeuron*_startCon=0;



MNeuron::MNeuron()
{
        _act=0.;
        mode=Null;
        cons=0;
}

MNeuron::MNeuron(const MUnitID&i)
{
        _act=0.;
        _id=i;
        mode=Null;
        cons=0;
}

MNeuron::~MNeuron()
{
        if(_startCon==this)_startCon=0;
        if(cons){
                MConnection*con;
                QListIterator<MConnection> it0(cons[0]);
                while((con=it0.current())!=0)delete con;
                QListIterator<MConnection> it1(cons[1]);
                while((con=it1.current())!=0)delete con;
        }
        if(cons)delete[]cons;
}

void MNeuron::draw(QPainter&p,const QRect&vis,float zoom)
{
        M2FRect rct(_pos,M2FSize(0.5,0.5));
        rct.move(0.25,0.25);
        QRect drw=(rct*=zoom);
        if(vis.intersects(drw)){
                p.fillRect(vis.intersect(drw),blue);
                if(_startCon==this){
                        p.setPen(QPen(red,1));
                        p.drawRect(drw);
                }
        }
        if(cons){
                QListIterator<MConnection> it(cons[0]);
                MConnection*con;
                while((con=it.current())!=0){
                        ++it;
                        con->draw(p,vis,zoom);
                }
        }
        if(mode==Move){
                int z=int(zoom*0.5);
                QRect drw(moveP.x()-(z*0.5),moveP.y()-(z*0.5),z,z);
                if(vis.intersects(drw)){
                        p.setPen(QPen(black,1,SolidLine));
                        p.drawRect(drw);
                }
        }
}

void MNeuron::update(const QRect&,float)
{
}

void MNeuron::networkEvent(MEventList&){}


void MNeuron::propagateDrawEvent(MDrawEvent&ev)
{
        M2FRect rct(_pos,M2FSize(0.5,0.5));rct.move(0.25,0.25);
        QRect phy=(rct*=ev.zoom());
        if(phy.contains(ev.pos())){
                if(ev.action()==MDrawEvent::Press&&ev.button()==LeftButton){
                        switch(ev.tool()){
                                case MDisplay::TDrag:
                                        moveP=ev.pos();
                                        mode=Move;
                                        ev.setHandled();
                                        connect(ev.widget(),ev.signal(),
                                          this,SLOT(drawEvent(MDrawEvent&)));
                                        break;
                                case MDisplay::TDelete:
                                        delete this;
                                        ev.setHandled();
                                        break;
                                case MDisplay::TAdd:
                                        //no neuron where one sits
                                        ev.setHandled();
                                        break;
                                case MDisplay::TModify:
                                        //draw menu
                                        break;
                                case MDisplay::TSelect:
                                        //how to do that? dont know.
                                        break;
                                case MDisplay::TConnect:
                                        if(_startCon){
                                                if(_startCon==this){
                                                        ev.setHandled();
                                                }else{
                                                        doCon(_startCon->doCon(this));
                                                        ev.setHandled();
                                                }
                                                _startCon=0;
                                        }else{
                                                _startCon=this;
                                                ev.setHandled();
                                        }
                                        break;
                        }
                }else 
                if(ev.action()==MDrawEvent::Press&&ev.button()==RightButton){
                        //draw menu
                }else
                if(ev.action()==MDrawEvent::Release&&ev.button()==LeftButton&&
                   ev.tool()==MDisplay::TConnect&&_startCon&&_startCon!=this){
                        doCon(_startCon->doCon(this));
                        ev.setHandled();
                        _startCon=0;
                }
        }else{//phy doesn't contain the mouse pointer
                if(cons){
                        QListIterator<MConnection>it(cons[0]);
                        MConnection*con;
                        while((con=it.current())!=0){
                                ++it;
                                con->propagateDrawEvent(ev);
                                if(ev.handled())break;
                        }
                        //later we should add code for cons[1] to
                        //cover connections going outside the subnet
                }
        }
}

void MNeuron::drawEvent(MDrawEvent&ev)
{
        switch(ev.action()){
                case MDrawEvent::Move:
                        moveP=ev.pos();
                        ev.setHandled();
                        break;
                case MDrawEvent::Release:
                        _pos.setXY(ev.pos().x()/ev.zoom(),ev.pos().y()/ev.zoom());
                        ev.setHandled();
                        mode=Null;
                        ev.widget()->disconnect(this,SLOT(drawEvent(MDrawEvent&)));
                        break;
                default: //this shouldn't happen, so we reset:
                        mDebug("GUI: oops! unknown event! in MNeuron::drawEvent\n");
                        disconnect(this,SLOT(drawEvent(MDrawEvent&)));
                        mode=Null;
                        break;
        }
}

MConnection*MNeuron::doCon(MNeuron*target)
{
        MConnection*con=new MConnection(this,target);
        if(!cons)cons=new QList<MConnection>[2];
        cons[0].append(con);
        return con;
}

void MNeuron::doCon(MConnection*con)
{
        if(!cons)cons=new QList<MConnection>[2];
        cons[1].append(con);
}

void MNeuron::deCon(MConnection*con)const
{
        if(cons){
                cons[0].remove(con);
                cons[1].remove(con);
        }
}

MConnection::MConnection(){}
MConnection::MConnection(const MConnectionID&){}
MConnection::MConnection(MNeuron*s,MNeuron*e)
{
        _start=s;
        _end=e;
        _weight=1.0;
}

MConnection::~MConnection()
{
        _start->deCon(this);
        _end->deCon(this);
}

void MConnection::draw(QPainter&p,const QRect&vis,float zoom)
{
        M2FPoint st(_start->_pos),en(_end->_pos);
        st.move(0.5,0.25);en.move(0.5,0.75);
        QPoint qst=(st*=zoom),qen=(en*=zoom);
        if(vis.contains(qst)||vis.contains(qen)){
                p.setPen(QPen(black,1));
                p.drawLine(qst,qen);
        }
}

void MConnection::update(const QRect&,float){}
void MConnection::networkEvent(MEventList&){}
void MConnection::drawEvent(MDrawEvent&){}

//this function evaluates whether x and y differ no more than e
static inline bool eps(double x,const double&y,const double&e)
{
        if((x<0.&&y<0.)||(x>0.&&y>0.))x-=y;else x+=y;
        return x>0.?x<=e:-x<=e;
}

void MConnection::propagateDrawEvent(MDrawEvent&ev)
{
        M2FPoint start(_start->_pos),end(_end->_pos),mouse(ev.pos());
        start.move(0.5,0.25);end.move(0.5,0.75);
        start*=ev.zoom();end*=ev.zoom();
        if(eps(start.hypot(end),start.hypot(mouse)+end.hypot(mouse),1.5)){
                if(ev.action()==MDrawEvent::Press){
                        switch(ev.tool()){
                                case MDisplay::TDelete:
                                        delete this;
                                        ev.setHandled();
                                        break;
                        }
                }
        }
}

MNetwork::MNetwork(){_net=0;}
MNetwork::MNetwork(uint32 n){_net=n;}
MNetwork::~MNetwork(){}
void MNetwork::draw(QPainter&p,const QRect&vis,float zoom)
{
        QObjectList *list=queryList("MEntity");
        QObjectListIt it(*list);
        MEntity *ent;
        while((ent=(MEntity*)it.current())!=0){
                ++it;
                ent->draw(p,vis,zoom);
        }
        delete list;
}

void MNetwork::update(const QRect&,float){}
void MNetwork::networkEvent(MEventList&){}

void MNetwork::drawEvent(MDrawEvent&ev)
{
}

void MNetwork::propagateDrawEvent(MDrawEvent&ev)
{
        //calculate 3d position
        if(_net==0){
                MPoint pos(0,0,0);
                pos.setXY(ev.pos().x()/ev.zoom(),ev.pos().y()/ev.zoom());
                ev.setPoint(pos);
        }
        //check whether it is our event
        if(_net!=0&&!(_room.inside2D(ev.point())))
                return;
        
        //propagate through children
        QObjectList *list=queryList("MEntity");
        QObjectListIt it(*list);
        MEntity *ent;
        while((ent=(MEntity*)it.current())!=0){
                ++it;
                ent->propagateDrawEvent(ev);
                if(ev.handled())break;
        }
        delete list;
        if(ev.handled())return;
        //check state/tool for special events
        if(ev.action()==MDrawEvent::Press)switch(ev.tool()){
                case MDisplay::TAdd:{
                        MNeuron*n=new MNeuron;
                        n->setLogPos(ev.point());
                        insertChild(n);
                        ev.setHandled();
                        break;
                }
                case MDisplay::TDelete:
                        //don't delete networks this way!
                        break;
                case MDisplay::TModify:
                        break;
                case MDisplay::TDrag:
                        if(_net==0)break; //don't move master, it's already everywhere
                        break;
                default:
                        mDebug("GUI: MNetwork: unimplemented tool %i\n",ev.tool());
                        break;
        }
}


