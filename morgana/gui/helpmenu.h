/*#*********************************************************************
 ** Macros for the "Help"-Menus
 **
 ** (c) Konrad Rosenbaum, 2000
 ** protected by the GNU GPL v.2 or any newer
 ***********************************************************************
 ** $Log: helpmenu.h,v $
 ** Revision 1.1  2000/09/21 18:40:52  pandur
 ** init
 **
 ***********************************************************************/
 
#ifndef M_HELPMENU_H
#define M_HELPMENU_H

#include <qpopupmenu.h>
#include "helpwin.h"

class QWidget;

/** returns a PopupMenu which contains the main entries for a help-menu*/
QPopupMenu*helpMenu(QWidget*,const QString&entry=(const char*)0,
        QObject*obj=0,const char*member=0);

//Makro which shortenes the implementation of help-methods
//usage: HELPMETHODS(class-name)
#define HELPMETHODS(cls) \
  void cls::helpIndex(){new MHelpWin("index.html");}\
  void cls::helpManual(){new MHelpWin("manual.html");}\
  void cls::aboutWin(){new MHelpWin("about.html");}
  
  
#endif //M_HELPMENU_H
