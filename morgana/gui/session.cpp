/*#*********************************************************************
 ** Session management
 **
 ** (c) Konrad Rosenbaum, 2000
 ** protected by the GNU GPL v.2 or any newer
 ***********************************************************************
 ** $Log: session.cpp,v $
 ** Revision 1.1  2000/09/21 18:40:52  pandur
 ** init
 **
 ***********************************************************************/

//Internationalization
#include "intl.h"

//Qt
#include <qsplitter.h>
#include <qobjectlist.h>
#include <qapplication.h>
#include <qtimer.h>

//own classes
#include "session.h"
#include "controlcenter.h"
#include <ipc/connection.h>
#include <ipc/package.h>
#include "debug.h"

//local system capabilities
#include <posix/inet.h>
#include <posix/process.h>
#include <posix/string.h>

MSession * MSession::first=0;

MSession::MSession(QWidget * parent, const char * name, WFlags f)
	:QWidget(parent,name,f)
{
        spawn("snns-serv");
        if(_pid<0)return;
        
        constructWidget();
}


MSession::MSession(IConnection*acon, QWidget * parent, const char * name, WFlags f)
	:QWidget(parent,name,f)
{
        _pid=-1;
        con=acon;
        
        constructWidget();
}

void MSession::constructWidget()
{
	popup=new QPopupMenu;
	popup->insertItem(i18n("&Terminate"),this,SLOT(stop()));
	popup->insertItem(i18n("&Activate"),this,SLOT(activate()));
        popup->insertSeparator();
        popup->insertItem(i18n("&Control Center"),this,SLOT(controlCenter()));
        popup->insertItem(i18n("&Properties ..."),this,"0nosuchslot()");
        popup->insertSeparator();
        popup->insertItem(i18n("ping-test"),this,SLOT(ping()));
	
	
	QSplitter *split=new QSplitter(this);
	lid=new MSessionLabel("LHOST",split);
	lid->setFrameStyle(QFrame::Box|QFrame::Sunken);
	lid->resize(100,lid->height());
	connect(lid,SIGNAL(mouseRelease(const QPoint&,ButtonState)),this,SLOT(mouseRelease(const QPoint&,ButtonState)));
	ltarget=new MSessionLabel("Port "+QString::number(con->peerPort()),split);
	ltarget->setFrameStyle(QFrame::Box|QFrame::Sunken);
	ltarget->resize(200,ltarget->height());
	ltarget->move(100,0);
	connect(ltarget,SIGNAL(mouseRelease(const QPoint&,ButtonState)),this,SLOT(mouseRelease(const QPoint&,ButtonState)));
	lstatus=new MSessionLabel("PID "+QString::number(_pid),split);
	lstatus->setFrameStyle(QFrame::Box|QFrame::Sunken);
	lstatus->resize(200,lstatus->height());
	lstatus->move(300,0);
	connect(lstatus,SIGNAL(mouseRelease(const QPoint&,ButtonState)),this,SLOT(mouseRelease(const QPoint&,ButtonState)));
	
	split->resize(500,lid->height());
	setMinimumHeight(lid->height());setMaximumHeight(lid->height());
	isactive=false;
	
	if(first){
		next=first;
		prev=first->prev;
		prev->next=this;
		first->prev=this;
		QColor c(gray);
		setBackgroundColor(c);
		lid->setBackgroundColor(c);
		ltarget->setBackgroundColor(c);
		lstatus->setBackgroundColor(c);
	} else {
		first=this;
		next=prev=this;
		activate();
	}
         
         //debug code
         connectPackage("ping",this,SLOT(pingRec(IPackage&)));
         QTimer::singleShot(1000,this,SLOT(login()));
}

void MSession::login()
{
        mDebug("GUI: sending login\n");
        IPackage p("login");con->send(p);
        //unrestrict package size and buffers
        con->setMaxPackage(0);
        con->setInputBufferSize(131072);//128kB
        con->setOutputBufferSize(131072);
}

MSession::~MSession()
{
        //first->next<-prev chain gets deallocated in stop()
        //kill process if any:
        mDebug("GUI: deconstructing session %s\n",(const char*)id());
        kill();
}

void MSession::mouseRelease(const QPoint&p,ButtonState button)
{
	if(button==RightButton)popup->popup(p);
	else if(button==LeftButton)activate();
}

void MSession::stop()
{
	bool svactive=isactive;
	if(isactive)deactivate();
	stopped(this);
	
	if(first==this)first=next;
	if(first==this)first=0;
	prev->next=next;
	next->prev=prev;
	next=prev=this;
	if(svactive&&first)first->activate();
	if(!first)nomoreSessions();
	
	delete this;
}

void MSession::activate()
{
	if(isactive)return;
	
	for(MSession*tmp=this->next;tmp!=this;tmp=tmp->next)
		tmp->deactivate();
		
	isactive=true;
	activated(this);
	
	QColor c(0,255,0);
	setBackgroundColor(c);
	lid->setBackgroundColor(c);
	ltarget->setBackgroundColor(c);
	lstatus->setBackgroundColor(c);
	repaint();
}

void MSession::deactivate()
{
	if(!isactive)return;
	
	isactive=false;
	deactivated(this);
	
	QColor c(gray);
	setBackgroundColor(c);
	lid->setBackgroundColor(c);
	ltarget->setBackgroundColor(c);
	lstatus->setBackgroundColor(c);
	repaint();
}

bool MSession::isActive()
{
	return isactive;
}

QString MSession::id()
{
        return "LHOST-"+QString::number(con->peerPort());
}

void MSession::spawn(const char*s)
{
        //create primary communication slot: pipes
        int o2i[2],i2o[2];
        if(::pipe(o2i)){
                _pid=-1;
                mDebug("GUI: pipe failed (%s)\n",strerror(errno));
                return;
        }
        if(::pipe(i2o)){
                ::close(o2i[0]);::close(o2i[1]);
                _pid=-1;
                mDebug("GUI: pipe failed (%s)\n",strerror(errno));
                return;
        }
        //create process
        _pid=::fork();
        //no success: dont create session object
        if(_pid<0){
                ::close(o2i[0]);::close(o2i[1]);
                ::close(i2o[0]);::close(i2o[1]);
                mDebug("GUI: no process, fork failed (%s)\n",strerror(errno));
                return;
        }
        //child: exec server or fail
        if(_pid==0){
                ::close(i2o[0]);::close(o2i[1]);
                ::close(STDOUT_FILENO);::close(STDIN_FILENO);
                ::dup2(i2o[1],STDOUT_FILENO);
                ::dup2(o2i[0],STDIN_FILENO);
                ::close(i2o[1]);::close(o2i[0]);
#warning stupid execl-call
                //find server
                QString tmp=findPath(s);
                execl((const char*)tmp,s,(char*)0);
                ::write(STDOUT_FILENO,"-1",2);
                mDebug("GUI-Child: execl failed\n");
                ::exit(1);
        }
        //parent: wait and then connect
        ::close(i2o[1]);::close(o2i[0]);
        char buf[64];//64 is paranoid, but works for sure
        sint32 i=::read(i2o[0],buf,64);
        if(i<=0){
                int status;
                ::close(i2o[0]);::close(o2i[1]);
                ::wait(&status);
                _pid=-1;
                mDebug("GUI: no data on pipe (%s)\n",strerror(errno));
                return;
        }
        buf[i]=0;
        i=atoi(buf);
        if(i<0){
                int status;
                ::close(i2o[0]);::close(o2i[1]);
                ::wait(&status);
                _pid=-1;
                mDebug("GUI: invalid data on pipe (%s)\n",buf);
                return;
        }
        int fd=socket(PF_INET,SOCK_STREAM,0);
        if(fd<0){
                ::close(i2o[0]);::close(o2i[1]);
                kill();
                int status;
                ::wait(&status);
                _pid=-1;
                mDebug("GUI: no socket (%s)\n",strerror(errno));
                return;
        }
        struct sockaddr_in sa;
        sa.sin_family=AF_INET;
        sa.sin_port=SWAB16(i);i=0;
        memcpy(&sa.sin_addr,&i,4);
        if(::connect(fd,(sockaddr*)&sa,sizeof(sa))){
                ::close(i2o[0]);::close(o2i[1]);
                kill();
                int status;
                ::wait(&status);
                _pid=-1;
                mDebug("GUI: connection failed (%s)\n",strerror(errno));
                return;
        }
        con=new IConnection(fd,"NN Server Alpha",IConnection::IPv4);
        con->setAutoClose(true);
        connect(con,SIGNAL(receive(IPackage&,IConnection*)),this,SLOT(receive(IPackage&)));
        mDebug("GUI: con-status(fd:%i)=%x\n",fd,con->error());
}

#define tp(x) strcat(strcat(strcpy(b,x),"/"),s);if((fd=::open(b,O_RDONLY))>=0){::close(fd);return b;}

QString MSession::findPath(QString s)
{
        char b[512],buf[512],*c,*d;
        const char *cc;
        int fd;
#ifdef MDEBUG
        tp("../snns")
#endif
        if((c=getenv("MDIR"))){
                tp(c);
        }
        QString tmp(qApp->argv()[0]);
        if(tmp.find('/')>=0){
                tmp.truncate(tmp.findRev('/'));
                cc=(const char*)tmp;
                tp(cc)
                tmp+="/bin";
                cc=(const char*)tmp;
                tp(cc)
        }
        if((c=getenv("PATH"))){
                while((d=strchr(c,':'))){
                        strncpy(buf,c,(d-c));
                        buf[(d-c)]=0;
                        tp(buf);
                        c=d+1;
                }
                tp(c);
        }
        if((c=getenv("HOME"))){
                tmp=c;tmp+="/morgana";
                cc=(const char*)tmp;
                tp(cc)
                tmp+="/bin";
                cc=(const char*)tmp;
                tp(cc)
                tmp=c;tmp+="/bin";
                cc=(const char*)tmp;
                tp(cc)
        }
        if((c=getenv("KDEDIR"))){
                tmp=c;tmp+="/bin";
                cc=(const char*)tmp;
                tp(cc)
        }
        tp("/usr/morgana")
        tp("/usr/morgana/bin")
        tp("/usr/bin")
        tp("/usr/local/morgana")
        tp("/usr/local/morgana/bin")
        tp("/usr/local/bin")
        tp("/opt/morgana")
        tp("/opt/morgana/bin")
        tp("/opt/bin")
        tp("/bin")
        
        //not found:
        return (QString)((char*)0);
}

#undef tp

void MSession::kill()
{
        //horribly dependent process: pid>0
        if(_pid>0){
                mDebug("GUI: killing pid %i\n",_pid);
                if(!(::kill(_pid,SIGTERM)))wait(0);
        }
        //unaffected process: 0, -1
        //unknown pid, but should be killed: -2
        if(_pid==-2){
#warning SIGTERM doesnt get through to server, 2b fixed!
                mDebug("GUI: sending term-signal to server\n");
                IPackage p("SIGNAL");p<<(QString)"terminate";
                con->send(p);
        }
        //reserved codes: pid < -2
        
}

int MSession::pid(){return _pid;}


void MSession::controlCenter()
{
        MControlCenter *cc=new MControlCenter(this);
        cc->show();
}

//////////////////////////////////////////////////////////
// Receiver

void MSession::receive(IPackage&p)
{
        mDebug("GUI: received %s (%li)\n",(const char*)p.name(),p.length());
        QObjectList*list=queryList("MSessionDeliver");
        QObjectListIt it(*list);
        QObject *obj;
        QString name=p.name();
        while((obj=it.current())!=0){
                ++it;
                if(*((MSessionDeliver*)obj) == name){
                        ((MSessionDeliver*)obj)->deliver(p);
                        delete list;
                        return;
                }
        }
        delete list;

        mDebug("GUI: last resort package: %s\n",(const char*)p.name());
        pLastResort(p);
}

bool MSession::connectPackage(QString name,const QObject*o,const char*m)
{
        //search for name
        QObjectList*list=queryList("MSessionDeliver");
        QObjectListIt it(*list);
        QObject *obj;
        bool b=true;//for compilers sake
        while ((obj=it.current())!=0){
                ++it;
                if(*((MSessionDeliver*)obj) == name){
                        b=connect(obj,SIGNAL(deliver(IPackage&)),o,m);
                        delete list;
                        mDebug("GUI: connectPackage(%s) %s -> (%s)%s::%s\n",
                        b?"TRUE":"FALSE",
                        (const char*)name,o->className(),o->name(),m);
                        return b;
                }
        }
        delete list;
        //create new child
        MSessionDeliver *d=new MSessionDeliver(name);
        insertChild(d);
        mDebug("GUI: connectPackage(%s) %s -> (%s)%s::%s\n",b?"true":"false",
         (const char*)name,d->className(),o->name(),m);
        b=connect(d,SIGNAL(deliver(IPackage&)),o,m);
        return b;
}

void MSession::pingRec(IPackage&)
{
        mDebug("GUI: ping received\n");
}

//////////////////////////////////////////////////////////
// Sender

void MSession::ping()
{
        IPackage p("ping");
        mDebug("GUI: ping send\n"); 
        p<<8UL;
        con->send(p);
}

void MSession::send(IPackage&p)
{
        con->send(p);
}

//////////////////////////////////////////////////////////
// Static

MSession*MSession::getActive()
{
        if(!first)return 0;
        MSession*tmp=first;
        do{
                if(tmp->isactive)return tmp;
                tmp=tmp->next;
        }while(tmp!=first);
        //shouldn't happen:
        return 0;
}

//////////////////////////////////////////////////////////
// Label

MSessionLabel::MSessionLabel( const QString &text, QWidget *parent, const char *name, WFlags f )
	:QLabel(text,parent,name,f)
{}

MSessionLabel::MSessionLabel( QWidget * buddy, const QString &t,QWidget * parent, const char * name, WFlags f)
	:QLabel(buddy,t,parent,name,f)
{}


void MSessionLabel::mouseReleaseEvent(QMouseEvent*me)
{
	mouseRelease(me->globalPos(),me->button());
}

//////////////////////////////////////////////////////////
// Delivery class

MSessionDeliver::MSessionDeliver(QString name)
{
        _name=name;
        connectCnt=0;
        mDebug("GUI: new deliverer for %s\n",(const char*)name);
}

MSessionDeliver::~MSessionDeliver()
{
        mDebug("GUI: del deliverer for %s\n",(const char*)_name);
}

QString MSessionDeliver::name()const
{
        return _name;
}

void MSessionDeliver::connectNotify(const char*s)
{
        connectCnt++;
        mDebug("GUI: %i delivery-connect %s -> signal %s\n",connectCnt,(const char*)_name,s);
}

void MSessionDeliver::disconnectNotify(const char*s)
{
        connectCnt--;
        mDebug("GUI: %i delivery-disconnect %s -> signal %s\n",connectCnt,(const char*)_name,s);
        if(connectCnt<=0)delete this;
}

bool MSessionDeliver::operator==(const QString&s)const
{
        return _name==s;
}

bool MSessionDeliver::operator==(const MSessionDeliver&d)const
{
        return _name==d._name;
}

