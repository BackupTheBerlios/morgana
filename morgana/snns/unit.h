#ifndef S_UNIT_H
#define S_UNIT_H

#if __GNUC__ == 2
//implementation in netset.cpp and ../gui/netevents.h
#pragma interface "unit.h"
#endif

#include <ipc/package.h>
#include <ipc/connection.h>

/**
 @short simple unit representation in packages
*/
class SUnit{
        public:
          SUnit(){mask=0;xpos=ypos=zpos=0;act=bias=iact=0.;id=0;}
          SUnit(const SUnit&u){operator=(u);}
          SUnit& operator=(const SUnit&u){
                  mask=u.mask;prototype=u.prototype;
                  xpos=u.xpos;ypos=u.ypos;zpos=u.zpos;
                  bias=u.bias;iact=u.iact;
                  actfunc=u.actfunc;outfunc=u.outfunc;
                  id=u.id;errcode=u.errcode;
                  name=u.name;
                  return *this;
          }
          
          /** The bitmasks are: 
          
          @li UPROTO - carries a prototype
          @li UPOS - carries a position (3D)
          @li UBIAS - c. bias
          @li UIACT - c. initial activation (SNNS only)
          @li UACTF - c. an activation function
          @li UOUTF - c. an output function
          @li UNAME - c. a name for that unit
          @li UACT - c. an activation value
          @li UNUM - c. a unit ID
          @li UERR - an error occurred, c. an error string
          */
          uint16 mask;
          QString prototype;
          sint32 xpos,ypos,zpos;
          float bias,iact,act;
          QString actfunc,outfunc;
          uint32 id;
          QString errcode;
          QString name;
};

#define UPROTO		0x0001
#define UPOS		0x0002
#define UBIAS		0x0004
#define UIACT		0x0008
#define UACTF		0x0010
#define UOUTF		0x0020
#define UNAME		0x0040
#define UACT		0x0080

#define UNUM		0x4000
#define UERR		0x8000


/**retrieves a unit out of a package*/
inline void getUnit(SUnit&u,IPackage&p)
{
        p>>u.mask;
        if(u.mask&UPROTO)p>>u.prototype;
                else u.prototype=(char*)0;
        if(u.mask&UPOS)  p>>u.xpos>>u.ypos>>u.zpos;
                else u.xpos=u.ypos=u.zpos=0;
        if(u.mask&UIACT) p>>u.iact;
                else u.iact=0.;
        if(u.mask&UACTF) p>>u.actfunc;
                else u.actfunc=(char*)0;
        if(u.mask&UOUTF) p>>u.outfunc;
                else u.outfunc=(char*)0;
        if(u.mask&UACT)  p>>u.act;
                else u.act=0.;
        if(u.mask&UNAME) p>>u.name;
                else u.name=(char*)0;
        if(u.mask&UNUM)  p>>u.id;
                else u.id=0;
}

/**stores a unit into a package*/
inline void setUnit(const SUnit&u,IPackage&p)
{
        p<<u.mask;
        if(u.mask&UPROTO)p<<u.prototype;
        if(u.mask&UPOS)  p<<u.xpos<<u.ypos<<u.zpos;
        if(u.mask&UIACT) p<<u.iact;
        if(u.mask&UACTF) p<<u.actfunc;
        if(u.mask&UOUTF) p<<u.outfunc;
        if(u.mask&UACT)  p<<u.act;
        if(u.mask&UNAME) p<<u.name;
        if(u.mask&UNUM)  p<<u.id;
}

/**sends a SNNS workaround package*/
inline void sendSNNS(IConnection*c,QString id,uint32 num)
{
        IPackage p("SNNS");
        p<<id<<num;
        c->send(p);
}

#define CHR(b,s) (strcpy((b),(const char*)(s)))

#endif //S_UNIT_H
