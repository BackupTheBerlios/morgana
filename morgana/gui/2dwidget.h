/*#*********************************************************************
 ** 2D Projection
 **
 ** (c) Konrad Rosenbaum, 2000
 ** protected by the GNU GPL v.2 or any newer
 ***********************************************************************
 ** $Log: 2dwidget.h,v $
 ** Revision 1.1  2000/09/21 18:40:52  pandur
 ** init
 **
 ***********************************************************************/

#ifndef M_2DWIDGET_H
#define M_2DWIDGET_H

#include <qscrollview.h>
#include <qpoint.h>

#include "entity.h"

class MDisplay;
class MSession;
class IPackage;
class MDrawEvent;
class QMouseEvent;
class QPaintEvent;

/**
  @short Display Class for 2D
  
  This class is the two-dimensional projection field for entities. And 
  it controls the entity-stack of the current display window.
*/
class M2DWidget:public QWidget
{
        Q_OBJECT
        
        public:
          M2DWidget(MDisplay*parent,MSession*session);
          ~M2DWidget();
          
        signals:
          /**emits the display-status to the status-line in the bottom of 
          the window.*/
          void status(const QString&);
          /**emits Events into the entity-stack*/
          void drawEvent(MDrawEvent&);
          
        public slots:
          /**returns the currently used tool-code
          @see MDisplay */
          void tool(int m){mode=m;}

        private slots:
          /**
          @deprecated
          receives packages from the server and emits them as events into
          the entity-stack*/
          void receive(IPackage&);
          
        protected:
          virtual void paintEvent(QPaintEvent*);
          virtual void mousePressEvent(QMouseEvent*);
          virtual void mouseReleaseEvent(QMouseEvent*);
          virtual void mouseMoveEvent(QMouseEvent*);
          virtual void mouseDoubleClickEvent(QMouseEvent*);
          virtual void connectNotify(const char*);
          virtual void disconnectNotify(const char*);
        
        private:
          /**connection to the session*/
          MSession*_session;
          /**pixel per network size unit*/
          float _zoom;
          /**state of action, currently used utility, 
          @see MDisplay
          for details*/
          int mode;
          /**
          @internal
          counts the amount of connections to drawEvent*/
          int cntDE;
          
          /**Main display unit: network #0 */
          MNetwork mnet;
};




#endif //M_2DWIDGET_H
