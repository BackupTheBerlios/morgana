/***********************************************************************
 ** 2-dimensional Display Main Widget
 **
 ** (c) Konrad Rosenbaum, 2000
 ** protected by the GNU GPL version 2
 ***********************************************************************
 ** $Log: 2dwidget.cpp,v $
 ** Revision 1.1  2000/09/21 18:40:52  pandur
 ** init
 **
 **
 ***********************************************************************/


#include "2dwidget.h"
#include "display.h"
#include "session.h"
#include "debug.h"

#include <qevent.h>
#include <qpainter.h>

//I'm not sure whether I'll keep it inherited from QWidget, so
//I define that for easier transportation to new inheritances
//this would be a nice extension for the C++ compiler - hint!hint!
#define inherited QWidget::

M2DWidget::M2DWidget(MDisplay*d,MSession*s)
        :QWidget(d),_zoom(25.0),cntDE(0),mnet(0)
{
        d->insertChild(this);
        _session=s;
        resize(400,400);
        s->connectPackage("get",this,SLOT(receive(IPackage&)));
        s->connectPackage("set",this,SLOT(receive(IPackage&)));
        s->connectPackage("listen",this,SLOT(receive(IPackage&)));
        
        //no tool selected and no active mode
        mode=0;
}

M2DWidget::~M2DWidget()
{
        mDebug("GUI: del 2dwidget\n");
}

void M2DWidget::receive(IPackage&)
{
}

void M2DWidget::paintEvent(QPaintEvent*pe)
{
        QPainter p(this);
        mnet.draw(p,pe->region().boundingRect(),_zoom);
}

void M2DWidget::connectNotify(const char*signal)
{
        mDebug("GUI: M2DWidget::connect(%s)\n",signal);
        inherited connectNotify(signal);
        if(!strcmp(signal,SIGNAL(drawEvent(MDrawEvent&))))cntDE++;
        mDebug("GUI: M2DWidget::cntDE=%i\n",cntDE);
}

void M2DWidget::disconnectNotify(const char*signal)
{
        mDebug("GUI: M2DWidget::disconnect(%s)\n",signal);
        inherited disconnectNotify(signal);
        if(signal==0)cntDE--;//disconnected from all signals
        else 
                if(!strcmp(signal,SIGNAL(drawEvent(MDrawEvent&))))cntDE--;
        mDebug("GUI: M2DWidget::cntDE=%i\n",cntDE);
}

void M2DWidget::mousePressEvent(QMouseEvent*ev)
{
        inherited mousePressEvent(ev);
        MDrawEvent m(ev,MDrawEvent::Press,_zoom,this,
                SIGNAL(drawEvent(MDrawEvent&)),mode);
        if(cntDE)drawEvent(m);
        else mnet.propagateDrawEvent(m);
        if(m.handled())repaint();
}

void M2DWidget::mouseReleaseEvent(QMouseEvent*ev)
{
        inherited mouseReleaseEvent(ev);
        MDrawEvent m(ev,MDrawEvent::Release,_zoom,this,
                SIGNAL(drawEvent(MDrawEvent&)),mode);
        if(cntDE)drawEvent(m);
        else mnet.propagateDrawEvent(m);
        if(m.handled())repaint();
}

void M2DWidget::mouseMoveEvent(QMouseEvent*ev)
{
        inherited mouseMoveEvent(ev);
        MDrawEvent m(ev,MDrawEvent::Move,_zoom,this,
                SIGNAL(drawEvent(MDrawEvent&)),mode);
        if(cntDE)drawEvent(m);
        else mnet.propagateDrawEvent(m);
        if(m.handled())repaint();
}

void M2DWidget::mouseDoubleClickEvent(QMouseEvent*ev)
{
        inherited mouseDoubleClickEvent(ev);
        MDrawEvent m(ev,MDrawEvent::DoubleClick,_zoom,this,
                SIGNAL(drawEvent(MDrawEvent&)),mode);
        if(cntDE)drawEvent(m);
        else mnet.propagateDrawEvent(m);
        if(m.handled())repaint();
}


