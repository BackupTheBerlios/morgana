/*#*********************************************************************
 ** Help Browser/Window
 **
 ** (c) Konrad Rosenbaum, 2000
 ** protected by the GNU GPL v.2 or any newer
 ***********************************************************************
 ** $Log: helpwin.cpp,v $
 ** Revision 1.1  2000/09/21 18:40:52  pandur
 ** init
 **
 ***********************************************************************/

#include "intl.h"
#include "debug.h"
#include "helpwin.h"

#include <posix/file.h>
#include <posix/string.h>
#include <posix/process.h>

#include <qapplication.h>
#include <qfile.h>
#include <qtextbrowser.h>
#include <qmime.h>
#include <qtextstream.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qhbox.h>

QString MHelpWin::directory;
QDict<QString>MHelpWin::dict;
MHelpWin* MHelpWin::current=0;

MHelpWin::MHelpWin(QString file):QWidget(0,0,WDestructiveClose)
{
        //global stuff
        if(directory.isNull())finddir();
        if(dict.isEmpty())filldict();
        
        previous=current;
        current=this;
        if(previous)previous->next=this;
        next=0;
        
        resize(400,400);
        
        QVBoxLayout *vbl=new QVBoxLayout(this);vbl->setAutoAdd(true);
        
        QHBox *hb=new QHBox(this);
        tb=new QTextBrowser(this);
        connect(tb,SIGNAL(textChanged()),this,SLOT(textChanged()));
        tb->mimeSourceFactory()->setExtensionType("html","text/html");

        QPushButton *bt;
        bt=new QPushButton(i18n("<<&Back"),hb);
        connect(bt,SIGNAL(clicked()),tb,SLOT(backward()));
        bt=new QPushButton(i18n("&Forward>>"),hb);
        connect(bt,SIGNAL(clicked()),tb,SLOT(forward()));
        bt=new QPushButton(i18n("[&Home]"),hb);
        connect(bt,SIGNAL(clicked()),tb,SLOT(home()));
        bt=new QPushButton(i18n("[&Index]"),hb);
        connect(bt,SIGNAL(clicked()),this,SLOT(index()));
        bt=new QPushButton(i18n("[&About]"),hb);
        connect(bt,SIGNAL(clicked()),this,SLOT(about()));
        bt=new QPushButton(i18n("[&Close]"),hb);
        connect(bt,SIGNAL(clicked()),this,SLOT(close()));
        
        
        if(directory==".nodir."){
                setCaption("Error");
                tb->setText(i18n("Error: No Help Available"));
        } else tb->setSource(directory+"/"+file);

        show();
        
        tb->setFocus();
}

MHelpWin::~MHelpWin()
{
        if(next){
                next->previous=previous;
                current=next;
        }else current=previous;
}
 
void MHelpWin::textChanged()
{
        QFile file(tb->source());
        if(file.open(IO_ReadOnly)){
                QString s("Help: "),tmp,*sp;
                char *l,lbuf[1024];l=lbuf;
                char buf[1024],*b;
                if(file.readLine(lbuf,1024)<0){
                        file.close();
                        setCaption("Help");
                        return;
                }
                file.close();
                //delete tags
                bool tag=false;
                for(b=buf;*l;l++){
                        if(*l=='\n');//ignore trailing newline
                        if(tag){
                                if(*l=='>')tag=false;
                        } else {
                                if(*l=='<')tag=true;
                                else *b++=*l;
                        }
                }
                *b=0;
                //copy
                tag=false;
                for(b=buf;*b;b++)
                        if(tag){
                                if(*b==';'){
                                        tag=false;
                                        sp=dict[tmp];
                                        if(!sp)s+="&"+tmp+";";
                                        else s+=*sp;
                                } else tmp+=*b;
                        } else {
                                if(*b=='&'){tag=true;tmp="";}
                                else s+=*b;
                        }
                setCaption(s);
        } else setCaption("Help");
}

void MHelpWin::index()
{
        tb->setSource(directory+"/index.html");
}

void MHelpWin::about()
{
        tb->setSource(directory+"/about.html");
}

///////////////////////////////
void MHelpWin::filldict()
{
        dict.insert("auml",new QString("ae"));
        dict.insert("Auml",new QString("Ae"));
        dict.insert("ouml",new QString("oe"));
        dict.insert("Ouml",new QString("Oe"));
        dict.insert("uuml",new QString("ue"));
        dict.insert("Uuml",new QString("Ue"));
        dict.insert("szlig",new QString("ss"));
        dict.insert("lt",new QString("<"));
        dict.insert("gt",new QString(">"));
        dict.insert("amper",new QString("&"));
        dict.insert("copy",new QString("(c)"));
        dict.insert("trade",new QString("[TM]"));        
}

#define td(x) if((fd=::open(strcat(strcpy(buf,x),"/.mdoc"),O_RDONLY))>=0){::close(fd);directory=x;return;}

void MHelpWin::finddir()
{
        char buf[256],*c,*d;
        const char*cc;
        int fd;
#ifdef MDEBUG
        char b[256];
        strcat(getcwd(b,240),"/../doc/online");
        td(b)
#endif
        if((c=getenv("MDOC"))){
                td(c);
        }
        QString tmp(qApp->argv()[0]);
        if(tmp.find('/')>=0){
                tmp.truncate(tmp.findRev('/'));tmp+="/doc";
                cc=(const char*)tmp;
                td(cc)
        }
        if((c=getenv("PATH"))){
                while((d=strchr(c,':'))){
                        strncpy(buf,c,(d-c));
                        buf[(d-c)]=0;
                        td(buf)
                        strcat(buf,"/doc");
                        td(buf)
                        c=d+1;
                }
                td(c);
        }
        if((c=getenv("HOME"))){
                tmp=c;tmp+="/doc/morgana";
                cc=(const char*)tmp;
                td(cc);
                tmp=c;tmp+="/morgana/doc";
                cc=(const char*)tmp;
                td(cc);
        }
        if((c=getenv("KDEDIR"))){
                tmp=c;tmp+="/share/doc/HTML/default/morgana";
                cc=(const char*)tmp;
                td(cc);
        }
        td("/usr/local/share/doc/HTML/default/morgana")
        td("/usr/local/share/doc/HTML/C/morgana")
        td("/usr/local/doc/morgana")
        td("/usr/share/doc/HTML/default/morgana")
        td("/usr/share/doc/HTML/C/morgana")
        td("/usr/morgana/doc")
        td("/opt/share/doc/HTML/default/morgana")
        td("/opt/share/doc/HTML/C/morgana")
        td("/opt/morgana/doc")
        td("/share/doc/HTML/default/morgana")
        td("/share/doc/HTML/C/morgana")
        td("/morgana/doc")
        //last resort:
        directory=".nodir.";
}

void MHelpWin::contextHelp(QString file)
{
        if(current)current->tb->setSource(directory+"/"+file);
        else new MHelpWin(file);
}

