/*#*********************************************************************
 ** Session management
 **
 ** (c) Konrad Rosenbaum, 2000
 ** protected by the GNU GPL v.2 or any newer
 ***********************************************************************
 ** $Log: session.h,v $
 ** Revision 1.1  2000/09/21 18:40:52  pandur
 ** init
 **
 ***********************************************************************/

#ifndef M_SESSION_H
#define M_SESSION_H

#include <internal/types.h>

#include <qobject.h>
#include <qwidget.h>
#include <qevent.h>
#include <qpoint.h>
#include <qpopupmenu.h>
#include <qlabel.h>
#include <qstring.h>

class IConnection;
class IPackage;
class MSessionLabel;


/** @short Sessions

    Handles display of the Session as well as the connection.
*/
class MSession:public QWidget
{
        Q_OBJECT
        
        public:
          MSession(QWidget * parent=0, const char * name=0, WFlags f=0 );
          MSession(IConnection*con, QWidget * parent=0, const char * name=0, WFlags f=0 );
          ~MSession();
          
          bool isActive();
          
          /**deliveres the currently active Session*/
          static MSession*getActive();
          
          /**deliveres the ID of the Session*/
          virtual QString id();
          
          /**PID of the server, -1 means error, 0 unknown PID*/
          int pid();
          
          /**connect object/slot for package delivery. The member must take 
          this parameter: void slotfunc(IPackage&), the package is delivered to
          you if it has the same name as you registered*/
          bool connectPackage(QString name,const QObject*receiver,const char*member);
          /**disconnects packages and receiver/member*/
          bool disconnectPackage(QString name,const QObject*receiver,const char*member);
          bool disconnectPackage(QString name,const QObject*receiver);
          
        public slots:
          /** 
          @internal 
          reaction on mouseclicks*/
          void mouseRelease(const QPoint&,ButtonState);
          /** stops the session*/
          void stop();
          /** activates this session*/
          void activate();
          /**use this to send packages*/
          void send(IPackage&);
          
        signals:
          void stopped(MSession*);
          void activated(MSession*);
          void deactivated(MSession*);
          void nomoreSessions();
          /**last resort for unclassified packages*/
          void pLastResort(IPackage&);
          
          
	private:
          void deactivate();
	  
	  QPopupMenu*popup;
	  MSessionLabel*lid,*ltarget,*lstatus;
	  
	  bool isactive;
	  
	  MSession *next,*prev;
	  
	  static MSession*first;
          
          /**Connection to the server of this session*/
          IConnection*con;
          /**PID of the server*/
          int _pid;
          /**only used by constructor: creates the server process*/
          void spawn(const char*);
          /**only used by destructor: kills server process*/
          void kill();
          /** @internal only to be used by constructor*/
          void constructWidget();
          //* @internal returns complete filename of that program
          QString findPath(QString);
          
        private slots:
          /** @internal receives data from the connection*/
          void receive(IPackage&);
          
          void ping();
          
          void login();
          
          void pingRec(IPackage&);
          
          void controlCenter();
          
};

/** 
  @internal 
  @short class to deliver packages
*/
class MSessionDeliver:public QObject
{
        Q_OBJECT
        public:
          MSessionDeliver(QString name);
          ~MSessionDeliver();
                    
          QString name()const;
          bool operator==(const MSessionDeliver&)const;
          bool operator==(const QString&)const;
          
          friend class MSession;

        protected:
          void connectNotify(const char*);
          void disconnectNotify(const char*);
        signals:
          void deliver(IPackage&);
        private:
          int connectCnt;
          QString _name;
};

/**
  @internal
  @short class to enable correct behaviour of the labels in the session widgets
*/
class MSessionLabel:public QLabel
{
	Q_OBJECT
	
	public:
	  MSessionLabel( const QString &text, QWidget *parent, const char *name=0, WFlags f=0 );
	  MSessionLabel( QWidget * buddy, const QString &,QWidget * parent, const char * name=0, WFlags f=0 );

	protected:
	  virtual void mouseReleaseEvent(QMouseEvent*);
	  
	signals:
	  void mouseRelease(const QPoint&,ButtonState);
};

#endif //M_SESSION_H
