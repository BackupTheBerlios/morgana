/***********************************************************************
 ** ControlCenter
 **
 ** (c) Konrad Rosenbaum, 2000
 ** protected by the GNU GPL v.2 or any newer
 ***********************************************************************
 ** History
 ** $Log: controlcenter.cpp,v $
 ** Revision 1.1  2000/09/21 18:40:52  pandur
 ** init
 **
 **
 ***********************************************************************/


//base decls
#include <internal/types.h>
#include "intl.h"
#include "debug.h"

//Qt
#include <qwidget.h>
#include <qtabwidget.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qvalidator.h>
#include <qlistbox.h>
#include <qcolor.h>

//IPC
#include <ipc/package.h>

//own
#include "controlcenter.h"
#include "session.h"
#include "helpwin.h"
#include "helpmenu.h"
#include "chat.h"
#include "display.h"

//"Yet not implemented" - macro:
#define yni this,SLOT(help())
#warning TODO: remove dummy-slot hack

MControlCenter::MControlCenter(MSession*s)
        :QMainWindow(0,"ControlCenter",WType_TopLevel|WDestructiveClose)
{
        ////////////////////////////////////////////////////////////
        // Caption/Inits
        setCaption("ControlCenter ("+s->id()+")");
        session=s;
        s->connectPackage("Control",this,SLOT(receive(IPackage&)));
        
        resize(500,300);
        
        
        ////////////////////////////////////////////////////////////
        // Menu
        
        _netmenu = new QPopupMenu;
        _netmenu->insertItem(i18n("&Open net"),yni);
        _netmenu->insertItem(i18n("&Save net"),yni);
        _netmenu->insertItem(i18n("Save net &As"),yni);
        _netmenu->insertSeparator();
        _netmenu->insertItem(i18n("&Info"),yni);
        _netmenu->insertItem(i18n("&Pruning"),yni);
        _netmenu->insertItem(i18n("Ca&scade"),yni);
        _netmenu->insertItem(i18n("&Kohonen"),yni);
        _netmenu->insertItem(i18n("In&version"),yni);
        _netmenu->insertSeparator();
        _netmenu->insertItem(i18n("&Close"),this,SLOT(close()));

        _patternmenu = new QPopupMenu;
        _patternmenu->insertItem(i18n("&Open pattern"),yni);
        _patternmenu->insertItem(i18n("Append &pattern"),yni);
        _patternmenu->insertItem(i18n("&Save pattern"),yni);
        _patternmenu->insertItem(i18n("Save pattern &As"),yni);
        _patternmenu->insertSeparator();
        _patternmenu->insertItem(i18n("Pattern &Editor"),yni);
        
        _displaymenu = new QPopupMenu;
        _displaymenu->insertItem(i18n("&2D Display"),this,SLOT(display()));
        _displaymenu->insertItem(i18n("&3D Display"),yni);
        _displaymenu->insertItem(i18n("&Error Graph"),yni);
        _displaymenu->insertItem(i18n("&Weights"),yni);
        _displaymenu->insertItem(i18n("&Projection"),yni);
        _displaymenu->insertItem(i18n("&Analyzer - necessary???"),yni);
        _displaymenu->insertSeparator();
        _displaymenu->insertItem(i18n("&Chat Window"),this,SLOT(chat()));
        
        _bignetmenu = new QPopupMenu;
        _bignetmenu->insertItem(i18n("general"),yni);
        _bignetmenu->insertItem(i18n("time-delay"),yni);
        _bignetmenu->insertItem(i18n("art1"),yni);
        _bignetmenu->insertItem(i18n("art2"),yni);
        _bignetmenu->insertItem(i18n("artmap"),yni);
        _bignetmenu->insertItem(i18n("kohonen"),yni);
        _bignetmenu->insertItem(i18n("jordan"),yni);
        _bignetmenu->insertItem(i18n("elman"),yni);
        _bignetmenu->insertItem(i18n("hopfield"),yni);
        _bignetmenu->insertItem(i18n("assoz"),yni);
        
        _helpmenu = helpMenu(this,i18n("&Control Center"),this,SLOT(help()));
        
        QMenuBar*menubar=menuBar();
        menubar->insertItem(i18n("&Networks"),_netmenu);
        menubar->insertItem(i18n("&Patterns"),_patternmenu);
        menubar->insertItem(i18n("&Display"),_displaymenu);
        menubar->insertItem(i18n("&BigNet"),_bignetmenu);
        menubar->insertSeparator();
        menubar->insertItem(i18n("&Help"),_helpmenu);
        menubar->show();
        
        ////////////////////////////////////////////////////////////
        // Tab
        
        QVBox*vbox=new QVBox(this);
        vbox->setSpacing(10);
        setCentralWidget(vbox);
        tab=new QTabWidget(vbox);
        vbox->setStretchFactor(tab,1);
        
        QWidget*tmp;
        
        tab->addTab(tmp=new QWidget,i18n("Learning"));
        tmp->setName("#learning");
        fillTabLearning(tmp);
        
        tab->addTab(tmp=new QWidget,i18n("Pattern"));
        tmp->setName("#pattern");//for help functions
        fillTabPattern(tmp);
        
        tab->addTab(tmp=new QWidget,i18n("Init"));
        tmp->setName("#init");
        fillTabInit(tmp);
        
        tab->addTab(tmp=new QWidget,i18n("Test"));
        tmp->setName("#test");
        fillTabTest(tmp);
        
        ////////////////////////////////////////////////////////////
        // bottom-end-Buttons
        
        QHBox*hbox=new QHBox(vbox);
        vbox->setStretchFactor(hbox,0);
        ((QBoxLayout*)(hbox->layout()))->addStretch(1);
        
        QButton *clb=new QPushButton(i18n("Close"),hbox);
        QButton *hb=new QPushButton(i18n("Help"),hbox);
        
        connect(clb,SIGNAL(clicked()),this,SLOT(close()));
        connect(hb,SIGNAL(clicked()),this,SLOT(help()));
}

void MControlCenter::fillTabLearning(QWidget*tmp)
{
        //temproary variables
        QBoxLayout *l1,*l2;
        QHBox *inthbox;
        QVBox *intvbox;
        QWidget *l;
        QComboBox *box;
        QLineEdit *ledit;
        
        //main Layout
        l1=new QVBoxLayout(tmp);
        l1->setMargin(5);
        
        //top line
        l2=new QHBoxLayout(l1);
        l2->addWidget(new QLabel(i18n("Steps:"),tmp),0);
        l2->addWidget(new QSpinBox(tmp),1);
        l2->addStretch(1);
        l2->addWidget(new QLabel(i18n("Method:"),tmp),0);
        l2->addWidget(box=new QComboBox(false,tmp),1);
        box->insertItem("Single Pattern");
        box->insertItem("All Patterns");
        box->insertItem("Shuffle Patterns");
        box->insertItem("Custom List");
        box->insertItem("Shuffle Custom List");
        
        //big middle layer
        l2=new QHBoxLayout(l1);
        
        //Functions
        intvbox=new QVBox(tmp);
        l2->addWidget(intvbox);
        intvbox->setFrameStyle(QFrame::Box|QFrame::Sunken);
        intvbox->setLineWidth(1);intvbox->setMidLineWidth(0);
        l=new QLabel(i18n("Functions:"),intvbox);
        l->setMaximumHeight(l->sizeHint().height());
        
        inthbox=new QHBox(intvbox);
        new QLabel(i18n("Learning"),inthbox);
        box=new QComboBox(false,inthbox);
        box->insertItem("Standard_Backpropagation");
        
        inthbox=new QHBox(intvbox);
        new QLabel(i18n("Ordering"),inthbox);
        box=new QComboBox(false,inthbox);
        box->insertItem("Topological_Order");
        
        inthbox=new QHBox(intvbox);
        new QLabel(i18n("Validation"),inthbox);
        box=new QComboBox(false,inthbox);
        box->insertItem("All Patterns");
        box->insertItem("Custom List");
        box->insertItem("Reverse Custum List");
        //box->insertItem("use Test settings");//will appear in Morgana-server
        
        //space at bottom
        new QWidget(intvbox);
        
        //Options
        intvbox=new QVBox(tmp);
        l2->addWidget(intvbox);
        intvbox->setFrameStyle(QFrame::Box|QFrame::Sunken);
        intvbox->setLineWidth(1);intvbox->setMidLineWidth(0);
        
        l=new QLabel(i18n("Parameters:"),intvbox);
        l->setMaximumHeight(l->sizeHint().height());
        
        inthbox=new QHBox(intvbox);
        new QLabel(i18n("Pattern no.:"),inthbox);
        new QSpinBox(inthbox);
        
        inthbox=new QHBox(intvbox);
        new QLabel(i18n("Learn #1:"),inthbox);
        ledit=new QLineEdit("0.0",inthbox);
        ledit->setValidator(new QDoubleValidator(ledit));
        
        new QCheckBox(i18n("Random noise"),intvbox);
        inthbox=new QHBox(intvbox);
        new QLabel(i18n("min %"),inthbox);
        new QLineEdit("0.0",inthbox);
        new QWidget(inthbox);
        new QLabel(i18n("max %"),inthbox);
        new QLineEdit("0.0",inthbox);
        new QCheckBox(i18n("Noise only on correlated hidden units"),intvbox);
        
        //space at bottom
        new QWidget(intvbox);
        
        //Bottom-Buttons
        l1->addSpacing(10);
        l2=new QHBoxLayout(l1);
        l2->addWidget(new QPushButton(i18n("Start"),tmp));
        l2->addWidget(new QPushButton(i18n("Stop"),tmp));
        
        //init layout/draw
        l1->activate();
}

void MControlCenter::fillTabPattern(QWidget*tmp)
{
        QBoxLayout *l1,*l2;
        QListBox *box;
        
        l1=new QHBoxLayout(tmp);
        l1->setMargin(5);
        
        //Left
        l2=new QVBoxLayout(l1);
        
        l2->addWidget(box=new QListBox(tmp));
        box->insertItem("List");
        box->insertItem("of");
        box->insertItem("original");
        box->insertItem("patterns");
        
        l2->addWidget(new QPushButton(i18n("Update"),tmp));
        
        
        //Middle
        l2=new QVBoxLayout(l1);
        
        l2->addWidget(new QPushButton("-->",tmp));
        l2->addWidget(new QPushButton("<--",tmp));
        
        //Right
        l2=new QVBoxLayout(l1);
        
        l2->addWidget(box=new QListBox(tmp));
        box->insertItem("List");
        box->insertItem("of");
        box->insertItem("customary");
        box->insertItem("chosen");
        box->insertItem("patterns");
        
        QHBox *hbox=new QHBox(tmp);
        l2->addWidget(hbox);
        new QPushButton("\\/",hbox);
        new QPushButton("/\\",hbox);
        
        l1->activate();
}

void MControlCenter::fillTabInit(QWidget*tab)
{
        QBoxLayout *l1;
        QVBox*vbox;
        QHBox*hbox;
        QFrame*frm;
        QComboBox*box;
        
        l1=new QHBoxLayout(tab);
        
        //Left
        l1->addWidget(vbox=new QVBox(tab));
        
        hbox=new QHBox(vbox);
        new QLabel("Function",hbox);
        box=new QComboBox(false,hbox);
        box->insertItem("Randomize Weights");
        box->insertItem("CC Weights");
        box->insertItem("Hebb");
        box->insertItem("RBF Weights");
        
        hbox=new QHBox(vbox);
        new QLabel("Param #1:",hbox);
        new QLineEdit("0.0",hbox);
        
        new QWidget(vbox);
        
        new QPushButton(i18n("Start"),vbox);
        
        //Line
        l1->addWidget(frm=new QFrame(tab),0);
        frm->setFrameStyle(QFrame::VLine|QFrame::Sunken);
        frm->setLineWidth(1);
        frm->setMidLineWidth(0);
        frm->resize(10,tab->height());
        frm->setFixedWidth(10);
        
        //Reight
        l1->addWidget(vbox=new QVBox(tab));
        
        new QLabel(i18n("Random Noise"),vbox);
        hbox=new QHBox(vbox);
        new QLabel(i18n("min %"),hbox);
        new QLineEdit("0.0",hbox);
        new QWidget(hbox);
        new QLabel(i18n("max %"),hbox);
        new QLineEdit("0.0",hbox);
        new QCheckBox(i18n("Noise only on correlated hidden units"),vbox);
        
        new QWidget(vbox);
        
        new QPushButton(i18n("Add Noise"),vbox);
        
        //Finalize
        l1->activate();
}

void MControlCenter::fillTabTest(QWidget*tab)
{
        QHBox *hbox;
        QComboBox *box;
        
        new QVBoxLayout(tab);
        tab->layout()->setAutoAdd(true);
        
        hbox=new QHBox(tab);
        new QLabel(i18n("Pattern"),hbox);
        box=new QComboBox(false,hbox);
        //dummy entries:
        box->insertItem("Pattern 1");
        box->insertItem("Pattern 2");
        new QWidget(hbox);
        
        new QWidget(tab);
        
        hbox=new QHBox(tab);
        new QPushButton(i18n("Start"),hbox);
        new QWidget(hbox);
}

MControlCenter::~MControlCenter()
{
}

void MControlCenter::help()
{
        MHelpWin::contextHelp("controlcenter.html"+
                (QString)tab->currentPage()->name());
}

HELPMETHODS(MControlCenter)

void MControlCenter::receive(IPackage&p)
{
        mDebug("GUI-CC: received %s\n",(const char*)p.name());
}

void MControlCenter::chat()
{
        (new MChat(session))->show();
}

void MControlCenter::display()
{
        (new MDisplay(session))->show();
}
