/*#********************************************************************
 ** Main File of Morgana
 **
 ** Konrad Rosenbaum, 2000
 ** protected by the GNU GPL v.2 or any newer
 **********************************************************************
 ** Implementation of main program and main window
 **********************************************************************
 ** $Log: main.cpp,v $
 ** Revision 1.1  2000/09/21 18:40:52  pandur
 ** init
 **
 **********************************************************************/

//environment
#include "intl.h"
#include <internal/types.h>
#include <posix/process.h>
#include "debug.h"

//Qt
#include <qapplication.h>
#include <qstatusbar.h>
#include <qvbox.h>
#include <qlayout.h>

//own
#include "main.h"
#include "controlcenter.h"
#include "session.h"
#include "connection.h"
#include "helpmenu.h"

#include <ipc/connection.h>


//"Yet not implemented" - macro:
#define yni this,SLOT(dummy())
#warning TODO: remove dummy-slot hack
#include <iostream.h>

MMain::MMain()
{
        
        _filemenu = new QPopupMenu;
        _filemenu->insertItem(i18n("&Start Session"),this,SLOT(startSession()));
        _filemenu->insertItem(i18n("&Connect to Session"),this,SLOT(connectSession()));
        _filemenu->insertSeparator();
        _filemenu->insertItem(i18n("C&ontrol Center"),this,SLOT(controlCenter()));
        _filemenu->insertSeparator();
        _filemenu->insertItem(i18n("&Exit"),this,SIGNAL(exit()));
        
        _optmenu = new QPopupMenu;
        _optmenu->insertItem(i18n("Options ...."),yni);
        
        _helpmenu = helpMenu(this);
        
        QMenuBar*menubar=menuBar();
        menubar->insertItem(i18n("&Session"),_filemenu);
        menubar->insertItem(i18n("&Options"),_optmenu);
        menubar->insertSeparator();
        menubar->insertItem(i18n("&Help"),_helpmenu);
        menubar->show();
        
        QStatusBar*statusbar=statusBar();
        statusbar->message("Yet no session loaded");
        
        QVBox*vb=new QVBox(this);
        vb->setMinimumSize(400,50);
        vb->setFrameStyle(QFrame::Panel|QFrame::Sunken);
        vb->setMargin(2);
       	((QBoxLayout*)vb->layout())->setDirection(QBoxLayout::TopToBottom);
        
        setCentralWidget(vb);
        
        mDebug("GUI: starting automatic session\n");
        startSession();
}

void MMain::dummy(){}

void MMain::controlCenter()
{
        MSession*s=MSession::getActive();
        if(!s)return;
        MControlCenter*cc=new MControlCenter(s);
        cc->show();
}

void MMain::startSession()
{
 	MSession *s=new MSession(centralWidget());
        if(s->pid()<0){
                delete s;
                return;
        }
 	connect(s,SIGNAL(activated(MSession*)),this,SLOT(sessionActivated(MSession*)));
        connect(s,SIGNAL(nomoreSessions()),this,SLOT(nomoreSessions()));
  	s->show();
        s->activate();
    	sessionActivated(s);
}

void MMain::connectSession()
{
        MConnect con(0,0,true);
        con.exec();
        if(!con.haveConnection())return;
        MSession*s=new MSession(con.connection(),centralWidget());
 	connect(s,SIGNAL(activated(MSession*)),this,SLOT(sessionActivated(MSession*)));
  	s->show();
        s->activate();
    	sessionActivated(s);
}


void MMain::sessionActivated(MSession*s)
{
	statusBar()->message("Session " + s->id() + " active");
}

void MMain::nomoreSessions()
{
        statusBar()->message("No active Session");
}

HELPMETHODS(MMain)

//////////////////////////////////////////////////////////////////////
// Main Program & global functions

void sig_child(int)
{
        //we aren't interested in sophisticated info about dead children
        //the socket tells us enough. 
        //		- please don't quote me on UNESCO meetings!
        mDebug("GUI: received SIGCHLD\n");
        waitpid(-1,0,WNOHANG|WUNTRACED);
}

int main(int argc,char**argv)
{
        struct sigaction sa;
        sa.sa_handler=sig_child;
        sa.sa_sigaction=0;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags=SA_NOMASK|SA_NOCLDSTOP|SA_RESTART;
        sa.sa_restorer=0;
        int e;
        if(sigaction(SIGCHLD,&sa,0)){
                e=errno;
                mDebug("GUI: sigaction failed (%i:%s)\n",e,strerror(e));
        }
        
        IConnection::setIgnoreSigPipe(true);
        
        QApplication app(argc,argv);
        
        MMain mm;
        mm.show();
        app.setMainWidget(&mm);
        
        app.connect(&mm,SIGNAL(exit()),SLOT(quit()));
        
        app.exec();
        
}
