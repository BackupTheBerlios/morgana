#ifndef M_NETIDS_H
#define M_NETIDS_H
/*#**********************************************************************
 ** generic classes for Network representation
 **
 ** (c) Konrad Rosenbaum, 2000
 ** protected by the GNU GPL v.2 or any newer
 ************************************************************************
 ** $Log: netids.h,v $
 ** Revision 1.1  2000/09/21 18:40:52  pandur
 ** init
 **
 **
 ************************************************************************/

#include <qpoint.h>
#include <qrect.h>
#include <qsize.h>

#include <internal/types.h>

#include <ipc/package.h>

#include "../snns/unit.h"


/*This #pragma is a special egcs 2.x extension. according to the C++ standard
  it isn't necessary, but it makes life much easier (esp. for the linker).
  
  More info: see the info pages for gcc, section "C++ extensions".
*/
#if __GNUC__ == 2
#pragma interface "netids.h"
#endif

class MFullSiteID;


/**
  @short Identifies an unit
  
  The GUI doesn't need to know how the ID works, this is topic to the
  server.
*/
class MUnitID
{
        public:
          MUnitID(){v1=v2=0;}
          MUnitID(IPackage&i){i>>v1>>v2;}
          MUnitID(const MUnitID&u){v1=u.v1;v2=u.v2;}
          ~MUnitID(){}
          
          bool operator==(const MUnitID&u)const{return v1==u.v1&&v2==u.v2;}
          bool operator!=(const MUnitID&u)const{return v1!=u.v1||v2!=u.v2;}
          
          const MUnitID& operator=(const MUnitID&u){
                  v1=u.v1;v2=u.v2;return *this;}
          const MUnitID& operator=(const MFullSiteID&s);
          
          void load(IPackage&i){i>>v1>>v2;}
          void store(IPackage&i)const{i<<v1<<v2;}
          
          uint32 net()const{return v1;}
          uint32 num()const{return v2;}
          
          void setNet(uint32 n){v1=n;}
          void setNum(uint32 n){v2=n;}
          
          void set(uint32 n,uint32 i){v1=n;v2=i;}
        
        private:
          uint32 v1,v2;
};

/**
  @short identifies a site or any other sub-unit element within that unit
*/
class MSiteID
{
        public:
          MSiteID(){v=0;}
          MSiteID(IPackage&i){i>>v;}
          MSiteID(const MSiteID&s){v=s.v;}
          ~MSiteID(){}
          
          bool operator==(const MSiteID&s)const{return v==s.v;}
          bool operator!=(const MSiteID&s)const{return v!=s.v;}
          
          const MSiteID& operator=(const MSiteID&s){v=s.v;return *this;}
          const MSiteID& operator=(const MFullSiteID&);
          
          void load(IPackage&i){i>>v;}
          void store(IPackage&i)const{i<<v;}
          
        private:
          uint32 v;
};


/**
  @short identifies a site and its unit
*/
class MFullSiteID
{
        public:
          MFullSiteID(){}
          MFullSiteID(IPackage&i):un(i),st(i){}
          MFullSiteID(const MUnitID&u,const MSiteID&s):un(u),st(s){}
          MFullSiteID(const MUnitID&u):un(u){}
          MFullSiteID(const MFullSiteID&f):un(f.un),st(f.st){}
          ~MFullSiteID(){}
          
          MUnitID unit()const{return un;}
          MSiteID site()const{return st;}
          
          bool operator==(const MFullSiteID&f)const{
                  return f.un==un&&f.st==st;}
          bool operator!=(const MFullSiteID&f)const{
                  return f.un!=un||f.st!=st;}
                  
          bool operator==(const MUnitID&f)const{return f==un;}
          bool operator!=(const MUnitID&f)const{return f!=un;}

          bool operator==(const MSiteID&f)const{return f==st;}
          bool operator!=(const MSiteID&f)const{return f!=st;}
          
          const MFullSiteID& operator=(const MFullSiteID&f){
                  un=f.un;st=f.st;return *this;}
          const MFullSiteID& operator=(const MUnitID&u){
                  un=u;return *this;}
          const MFullSiteID& operator=(const MSiteID&s){
                  st=s;return *this;}
          
          void load(IPackage&i){un.load(i);st.load(i);}
          void store(IPackage&i)const{un.store(i);st.store(i);}
          
        private:
          MUnitID un;
          MSiteID st;
};


inline const MUnitID&MUnitID::operator=(const MFullSiteID&s)
{
        return operator=(s.unit());
}

inline const MSiteID&MSiteID::operator=(const MFullSiteID&s)
{
        return operator=(s.site());
}

/**
  @short identifies a connection
*/
class MConnectionID
{
        public:
          MConnectionID(){}
          MConnectionID(IPackage&p):_start(p),_end(p){}
          MConnectionID(const MConnectionID&i):_start(i._start),_end(i._end){}
          MConnectionID(const MUnitID&s,const MUnitID&e):_start(s),_end(e){}
          MConnectionID(const MFullSiteID&s,const MFullSiteID&e):_start(s),_end(e){}
          ~MConnectionID(){}
          
          MFullSiteID start()const{return _start;}
          MFullSiteID end()const{return _end;}
          
          void setStart(const MFullSiteID&s){_start=s;}
          void setEnd(const MFullSiteID&e){_end=e;}
          
          bool operator==(const MConnectionID&c)const{
                  return _start==c._start && _end==c._end;}
          bool operator!=(const MConnectionID&c)const{
                  return _start!=c._start || _end==c._end;}
          
          void load(IPackage&i){_start.load(i);_end.load(i);}
          void store(IPackage&i)const{_start.store(i);_end.store(i);}
          
        private:
          MFullSiteID _start;
          MFullSiteID _end;
};
  

#endif //M_NETIDS_H
