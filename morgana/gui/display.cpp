/*#*********************************************************************
 ** 2D Display Class
 **
 ** (c) Konrad Rosenbaum, 2000
 ** protected by the GNU GPL v.2 or any newer
 ***********************************************************************
 ** $Log: display.cpp,v $
 ** Revision 1.1  2000/09/21 18:40:52  pandur
 ** init
 **
 **
 ***********************************************************************/

//own
#include "display.h"
#include <ipc/package.h>
#include "session.h"
#include "intl.h"
#include "helpmenu.h"
#include "2dwidget.h"

//Toolbar-Pixmaps
namespace xpm {
#include "pics/plus.xpm"
#include "pics/plusnet.xpm"
#include "pics/minus.xpm"
#include "pics/modify.xpm"
#include "pics/move.xpm"
#include "pics/select.xpm"
#include "pics/viewmag+.xpm"
#include "pics/viewmag-.xpm"
#include "pics/connect.xpm"
};

//Qt
#include <qtoolbar.h>
#include <qmenubar.h>
#include <qstatusbar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qpixmap.h>
#include <qobjectlist.h>
#include <qscrollview.h>

//Debugging
#include "debug.h"

//Local Tools
const int MDisplay::TAdd;
const int MDisplay::TAddNet;
const int MDisplay::TDelete;
const int MDisplay::TModify;
const int MDisplay::TSelect;
const int MDisplay::TDrag;
const int MDisplay::TZoomIn;
const int MDisplay::TZoomOut;
const int MDisplay::TConnect;

MDisplay::MDisplay(MSession*s)
        :QMainWindow(0,"Display",WType_TopLevel|WDestructiveClose)
{
        session=s;
        setCaption("Display ("+s->id()+")");
        
        statusBar();
        
        QPopupMenu *file=new QPopupMenu;
        file->insertItem(i18n("&Export as Picture"),0,0);
        file->insertItem(i18n("&Print"),0,0);
        file->insertSeparator();
        file->insertItem(i18n("&Close"),this,SLOT(close()));
        
        QPopupMenu *tools=new QPopupMenu;
        //Attention: Order is important
        tools->insertItem(i18n("&Drag"),this,SLOT(tool(int)),0,TDrag);
        tools->insertItem(i18n("&Add"),this,SLOT(tool(int)),0,TAdd);
        tools->insertItem(i18n("D&elete"),this,SLOT(tool(int)),0,TDelete);
        tools->insertItem(i18n("&Modify"),this,SLOT(tool(int)),0,TModify);
        tools->insertItem(i18n("&Selection"),this,SLOT(tool(int)),0,TSelect);
        tools->insertItem(i18n("Zoom &In"),this,SLOT(tool(int)),0,TZoomIn);
        tools->insertItem(i18n("Zoom &Out"),this,SLOT(tool(int)),0,TZoomOut);
        
        QMenuBar *m=menuBar();
        m->insertItem(i18n("&File"),file);
        m->insertItem(i18n("&Tools"),tools);
        m->insertSeparator();
        m->insertItem(i18n("&Help"),helpMenu(this));
        
        
        QToolBar *tb=_tools=new QToolBar(this);
        new MToolButton(QPixmap((const char**)xpm::move),i18n("drag neuron"),
                this,SLOT(tool(int)),tb,TDrag);

        new MToolButton(QPixmap((const char**)xpm::plus),i18n("add neuron"),
                this,SLOT(tool(int)),tb,TAdd);
                
        new MToolButton(QPixmap((const char**)xpm::plusnet),i18n("add network"),
                this,SLOT(tool(int)),tb,TAddNet);
                
        new MToolButton(QPixmap((const char**)xpm::minus),i18n("delete neuron"),
                this,SLOT(tool(int)),tb,TDelete);
                
        new MToolButton(QPixmap((const char**)xpm::connect),i18n("connect neurons"),
                this,SLOT(tool(int)),tb,TConnect);
                
        new MToolButton(QPixmap((const char**)xpm::modify),i18n("modify neuron"),
                this,SLOT(tool(int)),tb,TModify);
                
        new MToolButton(QPixmap((const char**)xpm::select),i18n("selection"),
                this,SLOT(tool(int)),tb,TSelect);
                
        new MToolButton(QPixmap((const char**)xpm::viewmag_plus),i18n("zoom in"),
                this,SLOT(tool(int)),tb,TZoomIn);
                
        new MToolButton(QPixmap((const char**)xpm::viewmag_minus),i18n("zoom out"),
                this,SLOT(tool(int)),tb,TZoomOut);
                
        QScrollView*sv=new QScrollView(this);
        setCentralWidget(sv);
        M2DWidget*w2=new M2DWidget(this,session);
        connect(this,SIGNAL(toolSet(int)),w2,SLOT(tool(int)));
        sv->addChild(w2);
        connect(w2,SIGNAL(status(const QString&)),
                statusBar(),SLOT(message(const QString&)));
                
        tool(TDrag);
        
        resize(400,400);
}

MDisplay::~MDisplay()
{
}

void MDisplay::tool(int num)
{
        mDebug("GUI: Display has chosen tool %i\n",num);
        _tool=num;
        //calibrating toolbar:
        QObject*obj;
        QObjectList*list=queryList("MToolButton");
        QObjectListIt it(*list);
        while((obj=it.current())!=0){
                ++it;
                ((MToolButton*)obj)->autoSet(num);
        }
        delete list;
        toolSet(num);
}

int MDisplay::tool()
{
        return _tool;
}

HELPMETHODS(MDisplay)

void MDisplay::help()
{
        new MHelpWin("display.html");
}

//////////////////////////////////////////////////////////////////
// Special ToolButtons
MToolButton::MToolButton(const QPixmap&pm,const QString&s,
        QObject*obj,const char*memb,QToolBar*parent,int num,const char*name)
        :QToolButton(parent,name)
{
        setPixmap(pm);
        setTextLabel(s);
        setToggleButton(true);
        connect(this,SIGNAL(clicked()),this,SLOT(activated()));
        connect(this,SIGNAL(activated(int)),obj,memb);
        buttonid=num;
}

void MToolButton::activated()
{
        activated(buttonid);
}

void MToolButton::autoSet(int id)
{
        setOn(id==buttonid);
}
