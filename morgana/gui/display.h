#ifndef M_DISPLAY_H
#define M_DISPLAY_H
/*#*********************************************************************
 ** 2D Display Class
 **
 ** (c) Konrad Rosenbaum, 2000
 ** protected by the GNU GPL v.2 or any newer
 ***********************************************************************
 ** $Log: display.h,v $
 ** Revision 1.1  2000/09/21 18:40:52  pandur
 ** init
 **
 ***********************************************************************/

#include <internal/types.h>

#include <qmainwindow.h>
#include <qtoolbutton.h>

class MSession;
class QToolBar;

/**
  @short Display Window
  
  This dialog offers a display for manipulation of networks.
*/
class MDisplay:public QMainWindow
{
        Q_OBJECT
        
        public:
          MDisplay(MSession*);
          ~MDisplay();
          
          /** returns the currently selected tool, refer to the T*-Constants
          for possible values*/
          int tool();
          
          static const int TAdd		=1000;
          static const int TDelete	=1001;
          static const int TModify	=1002;
          static const int TSelect	=1003;
          static const int TDrag	=1004;
          static const int TZoomIn	=1005;
          static const int TZoomOut	=1006;
          static const int TConnect	=1007;
          static const int TAddNet	=1008;

        signals:
          /**emitted whenever a tool is selected*/
          void toolSet(int);

        private slots:
          /**
          @internal
          receives the tool-number from the toolbar*/
          void tool(int);
          void helpIndex();
          void helpManual();
          void aboutWin();
          void help();
         
        private:
          MSession*session;
          QToolBar*_tools;
          int _tool;
};

/**
  @short Class which creates tool-buttons, which act like radiobuttons, not
  like checkboxes.
*/
class MToolButton:public QToolButton
{
        Q_OBJECT
        
        public:
         MToolButton(const QPixmap&pm,const QString&textlabel,
                 QObject*object,const char*member,QToolBar*parent,
                 int id=-1,const char*name=0);
                 
         void autoSet(int id);
         
        signals:
         void activated(int);
        
        private slots:
         void activated();
         
        private:
         int buttonid;
};

#endif //M_DISPLAY_H
