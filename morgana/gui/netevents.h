/*#***********************************************************************
 ** Network Events
 **
 ** (c) Konrad Rosenbaum, 2000
 ** protected by the GNU GPL v.2 or any newer
 *************************************************************************
 ** $Log: netevents.h,v $
 ** Revision 1.1  2000/09/21 18:40:52  pandur
 ** init
 **
 *************************************************************************/

#ifndef M_NETEVENTS_H
#define M_NETEVENTS_H


#if __GNUC__ == 2
#pragma interface "netevents.h"
#endif

#include "netids.h"
#include "dimtypes.h"
#include <qstring.h>
#include <qlist.h>
class IPackage;
class IConnection;
class SUnit;

/**
  @short Base event data class
  
  These are test-events whoms only reason is to show whether a server is able
  to communicate with a client they will be removed as soon as DIPC has
  implemented a new kind of communication.
  
  Indeed it is in doubt whether they will ever be seriously used.
*/
class MBaseEvent{
        public:
          MBaseEvent(){_delete=false;}
          virtual ~MBaseEvent(){}
          
          enum Type {Unit,Connection,Network,SNNS};
          
          /**returns the type of the event, must be overwritten by all
          inheriting classes*/
          virtual Type type()const=0;
          
          /**if this returns true it means "delete something" instead of
          "create or change something"*/
          virtual bool isDelete(){return _delete;}
          virtual void setDelete(bool d){_delete=d;}
          
          /**returns true if the event is marked as handled*/
          virtual bool handled()const{return _handled;}
          /**sets or removes the "this event is handled"-flag*/
          virtual void setHandled(bool handled=true){_handled=handled;}
          
        protected:
          bool _delete;
          bool _handled;
};

/**
  @short Unit Events
  
  They are able to cover almost all data SNNS can represent about a unit.
*/
class MUnitEvent:public MBaseEvent{
        public:
          MUnitEvent(){flags=0;net=0;}
          MUnitEvent(uint32 net,const SUnit&unit);
          MUnitEvent(uint32 n,uint32 u,bool isdel=false){net=n;id=u;_delete=isdel;}
          ~MUnitEvent(){}
          
          Type type()const{return Unit;}
          
          void getNet(uint32&n){n=net;}
          void setNet(uint32 n){net=n;}
          
          void getProto(QString&s)const{if(flags&0x1)s=proto;}
          void getPos(sint32&x,sint32&y,sint32&z)const{if(flags&0x2){x=px;y=py;z=pz;}}
          void getBias(float&b)const{if(flags&0x4)b=bias;}
          void getAct(float&a)const{if(flags&0x8)a=act;}
          void getActFunc(QString&af)const{if(flags&0x10)af=actf;}
          void getOutFunc(QString&of)const{if(flags&0x20)of=outf;}
          void getName(QString&nm)const{if(flags&0x40)nm=name;}
          void getId(uint32&i)const{if(flags&0x4000)i=id;}
          void getId(MUnitID&i)const{if(flags&0x4000)i.set(net,id);}

          bool hasProto()const{return flags&0x1;}
          bool hasPos()const{return flags&0x2;}
          bool hasBias()const{return flags&0x4;}
          bool hasAct()const{return flags&0x8;}
          bool hasActFunc()const{return flags&0x10;}
          bool hasOutFunc()const{return flags&0x20;}
          bool hasName()const{return flags&0x40;}
          bool hasId()const{return flags&0x4000;}

          void setProto(QString s){flags|=0x1;proto=s;}
          void setPos(sint32 x,sint32 y,sint32 z){flags|=0x2;px=x;py=y;pz=z;}
          void setBias(float b){flags|=0x4;bias=b;}
          void setAct(float a){flags|=0x8;act=a;}
          void setActFunc(QString af){flags|=0x10;actf=af;}
          void setOutFunc(QString of){flags|=0x20;outf=of;}
          void setName(QString nm){flags|=0x40;name=nm;}
          void setId(uint32 i){flags|=0x4000;id=i;}
          void setId(uint32 n,uint32 i){flags|=0x4000;net=n;id=i;}
          void setId(MUnitID i){flags|=0x4000;net=i.net();id=i.num();}
          
          bool hasError()const{return flags&0x8000;}
          void getError(QString&e)const{if(flags&0x8000)e=error;}
          void setError(const QString&e){flags|=0x8000;error=e;}
          
          bool operator==(const MUnitID&i)const{return id==i.num()&&net==i.net();}
          
          void toUnit(SUnit&)const;
          
        private:
          uint16 flags;
          uint32 id,net;
          QString proto,actf,outf,name,error;
          sint32 px,py,pz;
          float bias,act;
};

/**
  @short Connection Events
  
  Just tells which connections exist or die in a network.
*/
class MConnectionEvent:public MBaseEvent{
        public:
          MConnectionEvent(){_wgt=0.;}
          ~MConnectionEvent(){}
          
          Type type()const{return Connection;}
          
          MUnitID from(){return _from;}
          void setFrom(const MUnitID&f){_from=f;}
          MUnitID to(){return _to;}
          void setTo(const MUnitID&t){_to=t;}
          
          float weight(){return _wgt;}
          void setWeight(float w){_wgt=w;}
          
          bool operator==(const MConnectionID&i)const{return i.start()==_from&&i.end()==_to;}
          
        private:
          MUnitID _from,_to;
          float _wgt;
};

/**
  @short Network Events
  
  Yet not (and probably never) specified kind of event.
*/
class MNetworkEvent:public MBaseEvent{
        public:
          MNetworkEvent(){}
          ~MNetworkEvent(){}
          
          Type type()const{return Network;}
        
          bool operator==(uint32 i){return true;}
};

/**
  @short SNNS Events
  
  Workaround Events which cover the number-shift SNNS happens to initiate
  whenever a unit is pruned.
*/
class MSNNSEvent:public MBaseEvent{
        public:
          enum Action {Delete,Insert};
          MSNNSEvent(Action a=Delete,uint32 n=0){act=a;num=n;}
          ~MSNNSEvent(){}
          
          Type type(){return SNNS;}
          Action action(){return act;}
          void setAction(Action a){act=a;}
          
          uint32 number(){return num;}
          void setNumber(uint32 n){num=n;}
          
        private:
          Action act;
          uint32 num;
};

/**
  @short Events exchanged between server and GUI.
  
  Collects multiple Events which are exchanged between server and GUI.
*/
class MEventList{
        public:
          MEventList(){list.setAutoDelete(true);}
          MEventList(IPackage&);
          virtual ~MEventList(){}
          
          MUnitEvent* findUnit(const MUnitID&);
          MConnectionEvent* findConnection(const MConnectionID&);
          MNetworkEvent* findNetwork(uint32);
          
          void removeEvent(MBaseEvent*e){list.remove(e);}
          void addEvent(MBaseEvent*e){list.append(e);}
          
          void sendout(IConnection*)const;
          
          bool empty()const{return list.isEmpty();}
          
        protected:
          QList<MBaseEvent>list;
};

/**
  @short Events for Mouse actions on the display
*/
class MDrawEvent{
        public:
          enum Action {Press,Release,Move,DoubleClick};
          
          /** Initializes the event object.
          
          @li button: button of it, see @ref Qt::ButtonState
          @li state: state of it
          @li stateAfter: state after the event
          @li pos: Position in the widget
          @li action: button was pressed, hold or released
          @li zoom: current zoom factor of the widget
          @li widget,signal: pointer on the widget which emitted the event, 
              signal which emits following events
          */
          MDrawEvent(const QMouseEvent*ev=0,Action action=Press,float zoom=1.0,
                  QWidget*widget=0,const char*signal=0,int tool=0)
                  {_zoom=zoom;_btn=ev?ev->button():0;
                   _pos=ev?QPoint(ev->x(),ev->y()):QPoint(0,0);
                   _wgt=widget;_sig=signal;_act=action;_hdl=false;
                   _ste=ev?ev->state():0;_sta=ev?ev->stateAfter():0;
                   _tool=tool;}
          ~MDrawEvent(){}
          
          int button()const{return _btn;}
          int state()const{return _ste;}
          int stateAfter()const{return _sta;}
          const QPoint& pos()const{return _pos;}
          float zoom()const{return _zoom;}
          QWidget*widget()const{return _wgt;}
          const char*signal()const{return _sig;}
          Action action()const{return _act;}
          bool handled()const{return _hdl;}
          int tool()const{return _tool;}
          const MPoint point()const{return _point;}
          
          void setButton(int button){_btn=button;}
          void setState(int state){_ste=state;}
          void setStateAfter(int state){_sta=state;}
          void setPos(const QPoint&pos){_pos=pos;}
          void setZoom(float zoom){_zoom=zoom;}
          void setWidget(QWidget*widget){_wgt=widget;}
          void setSignal(const char*signal){_sig=signal;}
          void setAction(Action action){_act=action;}
          void setHandled(bool flag=true){_hdl=flag;}
          void setTool(int tool){_tool=tool;}
          void setPoint(const MPoint&p){_point=p;}
          
        private:
          bool _hdl;
          int _btn,_ste,_sta,_tool;
          QPoint _pos;
          float _zoom;
          QWidget*_wgt;
          const char*_sig;
          Action _act;
          MPoint _point;
};

#endif //M_NETEVENTS_H
