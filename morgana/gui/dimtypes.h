/*#***********************************************************************
 ** 2/3-dimensional types for internal conversions
 *************************************************************************
 ** $Log: dimtypes.h,v $
 ** Revision 1.1  2000/09/21 18:40:52  pandur
 ** init
 **
 **
 *************************************************************************/

#ifndef M_DIMTYPES_H
#define M_DIMTYPES_H

#if __GNUC__ == 2
#pragma interface "dimtypes.h"
#endif


#include <qpoint.h>
#include <qrect.h>

#include <internal/types.h>
#include <ipc/package.h>
  
#include <math.h>
  
/**
  @short 3dimensional points
  
  Synonymous types: MSize, MVector
*/
class MPoint
{
        public:
          MPoint(){_x=_y=_z=0;}
          MPoint(const MPoint&p){_x=p._x;_y=p._y;_z=p._z;}
          MPoint(sint32 x,sint32 y,sint32 z){_x=x;_y=y;_z=z;}
          ~MPoint(){}
          
          const MPoint& operator=(const MPoint&p){_x=p._x;_y=p._y;_z=p._z;return *this;}
          const MPoint& operator=(const QPoint&p){_x=p.x();_y=p.y();return *this;}
          
          bool operator==(const MPoint&p)const{return _x==p._x&&_y==p._y&&_z==p._z;}
          bool operator!=(const MPoint&p)const{return !operator==(p);}
          
          /**adds the Points like vectors*/
          MPoint operator+(const MPoint&p)const{
                  MPoint t(*this);
                  return t+=p;
          }
          const MPoint& operator+=(const MPoint&p){
                  _x+=p._x;
                  _y+=p._y;
                  _z+=p._z;
                  return *this;
          }
          
          /**handles the point as vector and returns the inverse vector*/
          MPoint operator-()const{return MPoint(-_x,-_y,-_z);}
          
          /**subtracts the "vector p" from this instance*/
          MPoint operator-(const MPoint&p)const {
                  MPoint t(*this);
                  return t-=p;
          }
          const MPoint&operator-=(const MPoint&p){
                  _x-=p._x;
                  _y-=p._y;
                  _z-=p._z;
                  return *this;
          }
          
          /**returns true if all dimensions of this point are arithmetically
          smaller than the ones of p*/
          bool operator<(const MPoint&p)const{
                  return _x<p._x&&_y<p._y&&_z<p._z;
          }
          
          bool operator<=(const MPoint&p)const{
                  return _x<=p._x&&_y<=p._y&&_z<=p._z;
          }
          
          sint32 x()const {return _x;}
          void setX(sint32 x){_x=x;}
          sint32 y()const {return _y;}
          void setY(sint32 y){_y=y;}
          sint32 z()const {return _z;}
          void setZ(sint32 z){_z=z;}
          
          QPoint xy()const {return QPoint(_x,_y);}
          void setXY(const QPoint&p){_x=p.x();_y=p.y();}
          void setXY(sint32 x,sint32 y){_x=x;_y=y;}
          QPoint yz()const {return QPoint(_y,_z);}
          void setYZ(const QPoint&p){_y=p.x();_z=p.y();}
          void setYZ(sint32 y,sint32 z){_y=y;_z=z;}
          QPoint xz()const {return QPoint(_x,_z);}
          void setXZ(const QPoint&p){_x=p.x();_z=p.y();}
          void setXZ(sint32 x,sint32 z){_x=x;_z=z;}
          
          void set(sint32 x,sint32 y,sint32 z){_x=x;_y=y;_z=z;}
          
          void load(IPackage&p){p>>_x>>_y>>_z;}
          void store(IPackage&p)const{p<<_x<<_y<<_z;}
          
        private:
          friend class MCube;
          sint32 _x,_y,_z;
          /**
          @internal
          for MCube, returns whether the dimensions are positive*/
          bool positive()const{return _x>=0&&_y>=0&&_z>=0;}
          /**
          @internal
          for MCube, returns whether x and y dimension are positive*/
          bool positive2d()const{return _x>=0&&_y>=0;}
};

typedef MPoint MSize;
typedef MPoint MVector;

/**
  @short 3dimensional volume
*/
class MCube
{
        public:
          MCube(){}
          MCube(const MPoint& start,const MSize& size):_st(start),_sz(size){
                  if(_sz._x<0){_st._x+=_sz._x;_sz._x=-_sz._x;}
                  if(_sz._y<0){_st._y+=_sz._y;_sz._y=-_sz._y;}
                  if(_sz._z<0){_st._z+=_sz._z;_sz._z=-_sz._z;}
          }
          MCube(const MCube&c):_st(c._st),_sz(c._sz){}
          ~MCube(){}
          
          const MCube& operator=(const MCube&c){_st=c._st;_sz=c._sz;return *this;}
          bool operator==(const MCube&c)const{return _st==c._st&&_sz==c._sz;}
          bool operator!=(const MCube&c)const{return _st!=c._st||_sz!=c._sz;}
          
          MPoint start()const{return _st;}
          MPoint pos()const{return _st;}
          MSize size()const{return _sz;}
          
          /**sets a new position*/
          void moveTo(const MPoint&p){_st=p;}
          /**moves the cube by p*/
          void move(const MPoint&p){_st+=p;}
          /**sets a new size*/
          void resize(MPoint p){
                  if(p._x<0){_st._x+=p._x;p._x=-p._x;}
                  if(p._y<0){_st._y+=p._y;p._y=-p._y;}
                  if(p._z<0){_st._z+=p._z;p._z=-p._z;}
                  _sz=p;
          }
          
          /**returns true if p is inside (including edges) the cube*/
          bool inside(MPoint p)const{
                  p-=_st;
                  return p.positive()&&p<=_sz;
          }
          
          /**returns true if p.xy is inside the cubes xy-coordinates*/
          bool inside2D(MPoint p)const{
                  p-=_st;
                  return p.positive2d()&&p._x<=_sz._x&&p._y<=_sz._y;
          }
          
        private:
          MPoint _st;
          MSize _sz;
};


/**
  @short 2D Float Point
  
  This one enables calculation in floating point space of points.
  Needed by the 2D-Display.
  
  Synonymous types: M2FSize
*/
class M2FPoint
{
        public:
          M2FPoint(){_x=_y=0.;}
          M2FPoint(float x,float y){_x=x;_y=y;}
          M2FPoint(const MPoint&p){_x=p.x();_y=p.y();}
          M2FPoint(const M2FPoint&p){_x=p._x;_y=p._y;}
          M2FPoint(const QPoint&p){_x=p.x();_y=p.y();}
          M2FPoint(const QSize&p){_x=p.width();_y=p.height();}
          
          operator QPoint()const{return QPoint((int)_x,(int)_y);}
          operator QSize()const{return QSize((int)_x,(int)_y);}
          
          float x()const{return _x;}
          void setX(float x){_x=x;}
          float y()const{return _y;}
          void setY(float y){_y=y;}
          float width()const{return _x;}
          void setWidth(float x){_x=x;}
          float height()const{return _y;}
          void setHeight(float y){_y=y;}
          
          void move(float x,float y){_x+=x;_y+=y;}
          
          const M2FPoint& operator=(const M2FPoint&p){_x=p._x;_y=p._y;return *this;}
          const M2FPoint& operator=(const QPoint&p){_x=p.x();_y=p.y();return *this;}
          const M2FPoint& operator+=(const M2FPoint&p){_x+=p._x;_y+=p._y;return *this;}
          const M2FPoint& operator+=(const QPoint&p){_x+=p.x();_y+=p.y();return *this;}
          M2FPoint operator-()const{return M2FPoint(-_x,-_y);}
          /**zooms/scales the vector represented by that point*/
          const M2FPoint& operator*=(float z){_x*=z;_y*=z;return *this;}
          M2FPoint operator*(float z)const{return M2FPoint(_x*z,_y*z);}
          bool operator==(const M2FPoint&p)const{return _x==p._x&&_y==p._y;}
          bool operator==(const QPoint&p)const{return (int)_x==p.x()&&(int)_y==p.y();}
          bool operator==(const QSize&p)const{return (int)_x==p.width()&&(int)_y==p.height();}
          bool operator!=(const M2FPoint&p)const{return _x!=p._x||_y!=p._y;}
          bool operator!=(const QPoint&p)const{return (int)_x!=p.x()||(int)_y!=p.y();}
          bool operator!=(const QSize&p)const{return (int)_x!=p.width()||(int)_y!=p.height();}
          
          /**returns the length of the line between this point and p*/
          float hypot(const M2FPoint&p)const{return ::hypot(_x-p._x,_y-p._y);}
          
        private:
          float _x,_y;
};

typedef M2FPoint M2FSize;

/**
  @short 2D Float Rectangle
*/

class M2FRect
{
        public:
          M2FRect(){}
          M2FRect(const M2FRect&r){_pos=r._pos;_size=r._size;}
          M2FRect(const M2FPoint&pos,const M2FSize&size){_pos=pos;_size=size;}
          M2FRect(const QRect&r){_pos=r.topLeft();_size=r.size();}
          
          operator QRect()const{return QRect((QPoint)_pos,(QSize)_size);}
          
          M2FPoint pos()const{return _pos;}
          void setPos(const M2FPoint&p){_pos=p;}
          M2FSize size()const{return _size;}
          void setSize(const M2FSize&s){_size=s;}
          
          void move(float x,float y){_pos.move(x,y);}
          
          const M2FRect& operator=(const M2FRect&r){_pos=r._pos;_size=r._size;return *this;}
          const M2FRect& operator=(const QRect&r){_pos=r.topLeft();_size=r.size();return *this;}
          const M2FRect& operator+=(const M2FRect&r){_pos+=r._pos;_size+=r._size;return *this;}
          const M2FRect& operator+=(const QRect&r){_pos+=r.topLeft();_size+=r.size();return *this;}
          const M2FRect& operator*=(float z){_pos*=z;_size*=z;return *this;}
          M2FRect operator*(float z){return M2FRect(_pos*z,_size*z);}
          
          bool operator==(const M2FRect&r)const{return _pos==r._pos&&_size==r._size;}
          bool operator==(const QRect&r)const{return _pos==r.topLeft()&&_size==r.size();}
          bool operator!=(const M2FRect&r)const{return _pos!=r._pos||_size!=r._size;}
          bool operator!=(const QRect&r)const{return _pos!=r.topLeft()||_size!=r.size();}
          
        private:
          M2FPoint _pos;
          M2FSize _size;
};

#endif //M_DIMTYPES_H
