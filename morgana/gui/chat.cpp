/*#*********************************************************************
 ** Chat: IPC-Test
 **
 ** (c) Konrad Rosenbaum, 2000
 ** protected by the GNU GPL v.2 or any newer
 ***********************************************************************
 ** $Log: chat.cpp,v $
 ** Revision 1.1  2000/09/21 18:40:52  pandur
 ** init
 **
 ***********************************************************************/

#include "intl.h"

#include "chat.h"
#include "session.h"
#include <ipc/package.h>

#include <qtextview.h>
#include <qhbox.h>
#include <qframe.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qscrollbar.h>

MChat::MChat(MSession*s):QVBox(0,0,WDestructiveClose)
{
        session=s;
        IPackage p("chat");
        p<<"listen";
        s->send(p);
        
        resize(400,300);
        setCaption("Chat on "+s->id());
        
        QHBox *hbox=new QHBox(this);
        new QLabel(i18n("Idendity"),hbox);
        ident=new QLineEdit("Nobody",hbox);
        
        viewer=new QTextView(this);
        
        hbox=new QHBox(this);
        input=new QLineEdit("",hbox);
        QPushButton *btn=new QPushButton(i18n("&Send"),hbox);
        connect(btn,SIGNAL(clicked()),this,SLOT(send()));
        connect(input,SIGNAL(returnPressed()),this,SLOT(send()));
        
        QFrame *fm=new QFrame(this);
        fm->setFrameStyle(QFrame::HLine|QFrame::Sunken);
        fm->setLineWidth(1);
        fm->setMidLineWidth(0);
        fm->setFixedHeight(10);
        
        hbox=new QHBox(this);
        btn=new QPushButton(i18n("&Close"),hbox);
        connect(btn,SIGNAL(clicked()),this,SLOT(close()));
        new QWidget(hbox);
        
        s->connectPackage("chat",this,SLOT(receive(IPackage&)));
}

MChat::~MChat()
{
        IPackage p("chat");
        p<<"unlisten";
        session->send(p);
}

void MChat::send()
{
        IPackage p("chat");
        p<<"send"<<ident->text()<<input->text();
        session->send(p);
        input->setText("");
}

void MChat::receive(IPackage&p)
{
        QString x,id,tx;
        p.pos(0);
        p>>x;
        if(x!="send")return;
        p>>id>>tx;
        viewer->append(id+": "+tx+"\n");
        viewer->ensureVisible(0,viewer->contentsHeight(),0,0);
        viewer->repaint(true);
}
