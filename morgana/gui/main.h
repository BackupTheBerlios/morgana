/*#*********************************************************************
 ** Main Window
 **
 ** (c) Konrad Rosenbaum, 2000
 ** protected by the GNU GPL v.2 or any newer
 ***********************************************************************
 ** $Log: main.h,v $
 ** Revision 1.1  2000/09/21 18:40:52  pandur
 ** init
 **
 ***********************************************************************/

#ifndef M_MAIN_H
#define M_MAIN_H

#include <qmainwindow.h>
#include <qmenubar.h>
#include <qpopupmenu.h>

#include "session.h"

/** 
  @short Main Window 
*/
class MMain:public QMainWindow
{
        Q_OBJECT
        
        public:
          MMain();
          
        private:
          QPopupMenu*_filemenu;
          QPopupMenu*_optmenu;
          QPopupMenu*_helpmenu;
          
        private slots:
          void dummy();
          void controlCenter();
          void startSession();
          void connectSession();
          void nomoreSessions();
          
          /**called whenever a session is highlighted by clicking on it*/
          void sessionActivated(MSession*);
          void aboutWin();
          void helpIndex();
          void helpManual();
         
        signals:
          void exit();
        
};
          

#endif //M_MAIN_H
