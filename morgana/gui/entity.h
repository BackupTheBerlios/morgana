/*#*********************************************************************
 ** Entities of neural networks
 **
 ** (c) Konrad Rosenbaum, 2000
 ** protected by the GNU GPL v.2 or any newer
 ***********************************************************************
 ** $Log: entity.h,v $
 ** Revision 1.1  2000/09/21 18:40:52  pandur
 ** init
 **
 ***********************************************************************/
#ifndef M_2DENTITY_H
#define M_2DENTITY_H

#include <qobject.h>
#include <internal/defs.h>

class QPainter;


#include "netevents.h"
#include "dimtypes.h"

/**
  @short Abstract class to represent entities in the 2D-Display
  
  Most methods in this class work on display coordinates. All methods
  carriing a "log" inside their name and the property logCoord work on
  logical coordinates. Display coordinates are the pixel-addresses used by
  the display widget. Logical coordinates are those which are used by the
  server, they are projected onto display coordinates using the zoom factor
  and dCoord.
  
  Side note:
  
  Entities waste a lot of space by beeing objects independent of each other.
  Currently this is all the space used for buffering coordinates and values
  plus the space inherited from QObject (for Qt 2.2 it is about 40 Bytes per
  instance). This can probably be worked around by using compound objects
  which represent dozens (or even thousands) of neurons and by deleting 
  objects which are out of sight. These goals are prepared but not used
  in this version. Thinkable are objects for very low zoom factors which add
  some fuzzyness by accumulating neighbored neurons/connections which can't
  be displayed on independent pixels.
*/
class MEntity:public QObject
{
        Q_OBJECT
        
        public:
          virtual ~MEntity(){};
          
          /** returns true if the point lies inside this entity
          
           redefined by MConnection*/
          virtual bool touched(const MPoint&p){return logRoom().inside(p);}
          
          /**returns logical position*/
          virtual MPoint logPos()const=0;
          virtual void setLogPos(const MPoint&)=0;
          
          /**returns logical size*/
          virtual MSize logSize()const=0;
          virtual void setLogSize(const MSize&)=0;
          
          /**returns the logical room used by this entity
          
           overwritten by most of the inheritants*/
          virtual MCube logRoom()const{return MCube(logPos(),logSize());}
          virtual void setLogRoom(const MCube&c){
                  setLogPos(c.pos());setLogSize(c.size());}
          
          /**draw-Events*/
          virtual void propagateDrawEvent(MDrawEvent&)=0;
          
        public slots:
          /**Request to redraw the entity.
          
          The parameters are: paint-device, visible area and zoom factor
          
          Needs to be redefined by all classes which inherit MEntity.*/
          virtual void draw(QPainter&,const QRect&,float){ABSTRACT(MEntity::draw);}
          
          /**Request to hide everything outside rect because it is out of 
          sight and to catch new data for the fields that have become visible.*/
          virtual void update(const QRect&,float){ABSTRACT(MEntity::hide);}
          
          /**Request to check an event and to handle it if appropriate.*/
          virtual void networkEvent(MEventList&){ABSTRACT(MEntity::networkEvent);}
          
          /**Request to handle a signal-bound draw-event*/
          virtual void drawEvent(MDrawEvent&){ABSTRACT(MEntity::drawEvent);}
          
        signals:
          /**Emitted whenever the Entity changes some properties*/
          void throwEvent(MEventList&);
          
};

class MConnection;

/**
  @short represents a single neuron/unit as used by SNNS
*/
class MNeuron:public MEntity
{
          Q_OBJECT
        public:
          MNeuron();
          MNeuron(const MUnitID&);
          ~MNeuron();
          
          bool touched(const MPoint&p){return _pos==p;}
          
          MPoint logPos()const{return _pos;}
          void setLogPos(const MPoint&p){_pos=p;}
          
          MSize logSize()const{return MSize(1,1,1);}
          void setLogSize(const MSize&){}
          
          MCube logRoom()const{return MCube(_pos,MSize(1,1,1));}
          void setLogRoom(const MCube&c){_pos=c.pos();}
          
          virtual void propagateDrawEvent(MDrawEvent&);
          
        public slots:
          void draw(QPainter&,const QRect&,float);
          void update(const QRect&,float);
          void networkEvent(MEventList&);
          void drawEvent(MDrawEvent&);
          
        private:
          friend MConnection;
          /**
          @internal
          Position in the Display*/
          MPoint _pos;
          /**
          @internal
          ID of this Neuron to identify it in the server*/
          MUnitID _id;
          /**
          @internal
          current activation*/
          float _act;
          
          /**
          @internal
          (drag-)mode of the Neuron*/
          enum {Null=0,Move} mode;
          
          /** 
          @internal
          Lists of connections to other MNeuron's.
          cons[0]=in cons[1]=out*/
          QList<MConnection> *cons;
          /**
          @internal
          start-Neuron calls this on building connections*/
          MConnection*doCon(MNeuron*);
          /**
          @internal
          target-Neuron calls this on building connections*/
          void doCon(MConnection*);
          /**
          @internal
          used by destructor of MConnection */
          void deCon(MConnection*)const;
          
          /**
          @internal
          temporary variable for mouse-movement*/
          static QPoint moveP;
};


/**
  @short represents a single connection between two neurons.
*/  
class MConnection:public MEntity{
        Q_OBJECT
        public:
          MConnection();
          MConnection(const MConnectionID&);
          MConnection(MNeuron*,MNeuron*);
          ~MConnection();
          
          MPoint logPos()const{return MPoint(0,0,0);}
          void setLogPos(const MPoint&){}
          
          MSize logSize()const{return MSize(0,0,0);}
          void setLogSize(const MSize&){}
          
          MCube logRoom()const{return MCube(logPos(),logSize());}
          void setLogRoom(const MCube&){}
          
          const MNeuron* start()const{return _start;}
          const MNeuron* end()const{return _end;}
          void setStart(const MNeuron*p){_start=p;}
          void setEnd(const MNeuron*p){_end=p;}
          
          virtual void propagateDrawEvent(MDrawEvent&);
          
        public slots:
          void draw(QPainter&,const QRect&,float);
          void update(const QRect&,float);
          void networkEvent(MEventList&);
          void drawEvent(MDrawEvent&);
        
        private:
          MConnectionID _id;
          /**
          @internal
          current weight of the Connection*/
          float _weight;
          const MNeuron*_start;
          const MNeuron*_end;
};

/**
  @short represents a complete Layer with uniform neurons which
  are organized in a matrix.
*/
class MLayer:public MEntity
{
};

/**
  @short represents a network which logically groups other entities
  
  Currently there is only one network, which covers the whole system.
*/
class MNetwork:public MEntity
{
        Q_OBJECT
        public:
          MNetwork();
          MNetwork(uint32);
          ~MNetwork();
          
          MPoint logPos()const{return _room.pos();}
          void setLogPos(const MPoint&p){_room.moveTo(p);}
          
          MSize logSize()const{return _room.size();}
          void setLogSize(const MSize&s){_room.resize(s);}
          
          MCube logRoom()const{return _room;}
          void setLogRoom(const MCube&c){_room=c;}
          
          virtual void propagateDrawEvent(MDrawEvent&);
          
        public slots:
          void draw(QPainter&,const QRect&,float);
          void update(const QRect&,float);
          void networkEvent(MEventList&);
          void drawEvent(MDrawEvent&);
        
        private:
          /**
          @internal
          network ID of this network, to identify it in the server*/
          uint32 _net;
          MCube _room;
};

#endif //M_2DENTITY_H
