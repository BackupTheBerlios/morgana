#ifndef IPC_ORB_H
#define IPC_ORB_H
/*#*************************************************************************
 ** local ORB $Revision: 1.1 $
 ***************************************************************************
 ** (c) Konrad Rosenbaum, 2000
 ** protected by the GNU GPL version 2 or any newer
 ***************************************************************************
 ** History:
 **
 ** $Log: orb.h,v $
 ** Revision 1.1  2000/11/25 15:23:35  pandur
 ** orb added, some new code
 **
 ***************************************************************************/

#include <qobject.h>

//optimize code size for GNU C 2.9x
#if __GNUC__ == 2
#pragma interface "ipc/orb.h"
#endif



class QDict<T>;
/**
   @short ORB of Morgana's IPC
   
   Any instance can be used to access the static instance of it, which
   holds the real data. Therefore only one ORB can be hold per process.
   
   This class is an interface into the local object repository. All
   local objects, signals and slots are registered here. Connections
   call it to control remote requests.
   
   Additionally it serves as the source for any authentification.
*/
class IORB
{
	public:
	  /**create a new instance to access the ORB*/
	  IORB();
	  ~IORB();
	  
	  static bool registerMethod(const QString&rmeth,QObject*lobj,char*method);
	  static bool unregisterMethod(const QString&rmeth);
	  static bool registerObject(const QString&obj,QObject*obj=0,bool allowothers=true);
	
	private:
          /**used by the ORB itself to instantiate the global one*/
	  IORB(bool);
          /**Am I the one? The real one?*/
	  bool isstatic=false;
          /**The one.*/
	  static IORB orb(true);
          /**List of Objects*/
	  static QDict<void> *loo=0;
          /**List of Methods*/
	  static QDict<void> *lom=0;
};

class IRemote;
/**
   @short abstract interface for complex serializable classes
*/
class ISerializable
{
	public:
	  virtual void serialize(IRemote&)const=0;
	  virtual void deserialize(IRemote&)=0;
};



//Makro for remote serialization
#define RPARAM(args...) (IRemote::getInstance(), ## args)
#define RVOID (IRemote::getInstance())

/**
   @short Parameter class for remotely callable methods
*/
class IRemote
{
	public:
	  IRemote(){blen=bpos=0;buffer=0;}
	  
	  static IRemote getInstance(){return 0;}
	 
	  IRemote& operator,(uint8 i){return operator<<(i);}
	  IRemote& operator,(uint16 i){return operator<<(i);}
	  IRemote& operator,(uint32 i){return operator<<(i);}
	  IRemote& operator,(sint8 i){return operator<<(i);}
	  IRemote& operator,(sint16 i){return operator<<(i);}
	  IRemote& operator,(sint32 i){return operator<<(i);}
	  IRemote& operator,(const QString&s){return operator<<(s);}
	  IRemote& operator,(const char*s){return operator<<(s);}
	  IRemote& operator,(bool b){return operator<<(b);}
	  IRemote& operator,(const ISerializable&s){return operator<<(s);}

	  IRemote& operator<<(uint8 i)
	  {
	      alloc(1);
	      memcpy(buffer+bpos++,&i,1);
	      return *this;
	  }
	  IRemote& operator<<(uint16 i)
	  {
	      alloc(1);
	      memcpy(buffer+bpos,&i,2);bpos+=2;
	      return *this;
	  }
	  IRemote& operator<<(uint32 i)
	  {
	      alloc(1);
	      memcpy(buffer+bpos,&i,4);bpos+=4;
	      return *this;
	  }

	  IRemote& operator<<(sint8 i)
	  {
	      alloc(1);
	      memcpy(buffer+bpos++,&i,1);
	      return *this;
	  }

	  IRemote& operator<<(sint16 i)
	  {
	      alloc(1);
	      memcpy(buffer+bpos,&i,2);bpos+=2;
	      return *this;
	  }

	  IRemote& operator<<(sint32 i)
	  {
	      alloc(1);
	      memcpy(buffer+bpos,&i,4);bpos+=4;
	      return *this;
	  }

	  IRemote& operator<<(const QString&s)
          {
                  QCString cs=s.utf8();
                  uint32 len;
                  if(cs.isNull()){
                          len=0xffffffff;
                          alloc(4);
                          memcpy(buffer+bpos,&len,4);
                          bpos+=4;
                  }else{
                          len=cs.length();
                          alloc(4+len);
                          memcpy(buffer+bpos,&len,4);
                          memcpy(buffer+bpos,(const char*)cs,len);
                          bpos+=4+len;
                          
                  }
                  return *this;
          }
          
	  IRemote& operator<<(const char*s)
	  {
	      return operator<<(QString(s));
	  }
	  
	  IRemote& operator<<(bool b)
	  {
	      alloc(1);uint8 i=b?0xff:0
	      memcpy(buffer+bpos++,&i,1);
	      return *this;
	  }

	  IRemote& operator<<(const ISerializable&s)
	  {
	          s.serialize(*this);
                  return *this;
	  }
	  
	  IRemote& operator>>(uint8&i)
          {
                  if((bpos+1)<blen){
                          memcpy(&i,buffer,1);
                          bpos++;
                  }
                  return *this;
          }
                          
	  IRemote& operator>>(uint16&i)
          {
                  if((bpos+2)<blen){
                          memcpy(&i,buffer,2);
                          bpos+=2;
                  }
                  return *this;
          }

	  IRemote& operator>>(uint32&i)
          {
                  if((bpos+4)<blen){
                          memcpy(&i,buffer,4);
                          bpos+=4;
                  }
                  return *this;
          }

	  IRemote& operator>>(sint8&i)
          {
                  if((bpos+1)<blen){
                          memcpy(&i,buffer,1);
                          bpos++;
                  }
                  return *this;
          }

	  IRemote& operator>>(sint16&i)
          {
                  if((bpos+2)<blen){
                          memcpy(&i,buffer,2);
                          bpos+=2;
                  }
                  return *this;
          }

	  IRemote& operator>>(sint32&i)
          {
                  if((bpos+4)<blen){
                          memcpy(&i,buffer,4);
                          bpos+=4;
                  }
                  return *this;
          }

	  IRemote& operator>>(QString&s)
          {
                  uint32 len;
                  if((bpos+4)>blen)return *this;
                  memcpy(&len,buffer,4);
                  if(len==0xffffffff)s=QString((char*)0);
                  bpos+=4;
                  if((bpos+len)>blen)return *this;
                  s=QString::fromUtf8((const char*)buffer,len);
                  bpos+=len;
          }
          
	  IRemote& operator>>(bool&b)
          {
                  if((bpos+1)<blen){
                          uint8 i,j=0;
                          memcpy(i,buffer,1);
                          bpos++;
                          //count the bits:
                          for(int k=0;k<8;k++)
                                  if((i>>k)&1)j++;
                          //if more than 4 are set we may assume it was
                          //formerly a 0xff, which was our value for true:
                          b=j>4;
                  }
                  return *this;
          }

	  IRemote& operator>>(ISerializable&s)
          {
                  s.deserialize(*this);
                  return *this;
          }
	  
          bool isend(int within=0)
          {
                  return (bpos+within)>=blen;
          }
	  
	private:
          friend class IBuffer;
	  /**
	     @internal
	     @see IRemote::getInstance()
	  */
	  IRemote(int){blen=bpos=0;buffer=0;}
	  
	  void*buffer;
	  int blen,bpos;
#warning IRemote needs error handling for alloc
 	  bool alloc(int inc)
	  {
	          register void *b;
		  register int l=blen;
		  inc+=bpos;
	          if(blen<inc){
		          while(l<inc)l+=16;
	                  b=realloc(buffer,l);
			  if(b==0)return false;
			  blen=l;buffer=b;
		  }
		  return true;
	  }
	  
};

/**
  @short simple buffer class
*/
class IBuffer:public ISerializable
{
        public:
          IBuffer(uint32 size){sz=size;buf=malloc(sz);}
          IBuffer(const void*b,uint32 size)
          {
                  sz=size;
                  buf=malloc(sz);
                  memcpy(buf,b,sz);
          }
          
          ~IBuffer(){free(buf);}
          
          void set(const void*b)
          {
                  memcpy(buf,b,sz);
          }
          
          void get(void*b)const
          {
                  memcpy(b,buf,sz);
          }
          
          uint32 length()const{return sz;}
          
          void serialize(IRemote&r)const
          {
                  r<<sz;
                  r.alloc(sz);
                  memcpy(r.buffer+r.bpos,buf,sz);
                  r.bpos+=sz;
          }
          
          void deserialize(IRemote&r)
          {
                  r>>sz;
                  free(buf);buf=malloc(sz);
                  if((r.bpos+sz)>r.blen)return;
                  memcpy(buf,r.buffer+r.bpos,sz);
                  r.bpos+=sz;
          }
          
        private:
          void*buf;
          uint32 sz;
          
};

#endif //IPC_ORB_H
