/*#*********************************************************************
 ** The "Help"-Menus
 **
 ** (c) Konrad Rosenbaum, 2000
 ** protected by the GNU GPL v.2 or any newer
 ***********************************************************************
 ** $Log: helpmenu.cpp,v $
 ** Revision 1.1  2000/09/21 18:40:52  pandur
 ** init
 **
 ***********************************************************************/
 
#include "helpmenu.h"
#include "intl.h"

#include <qwidget.h>
#include <qpopupmenu.h>


QPopupMenu*helpMenu(QWidget*win,const QString&entry,QObject*obj,const char*mem)
{
        QPopupMenu*_helpmenu = new QPopupMenu;
        if(!entry.isNull()){
                _helpmenu->insertItem(entry,obj,mem);
        }
        _helpmenu->insertItem(i18n("&Help Index"),win,SLOT(helpIndex()));
        _helpmenu->insertItem(i18n("&Manual"),win,SLOT(helpManual()));
        _helpmenu->insertSeparator();
        _helpmenu->insertItem(i18n("&About"),win,SLOT(aboutWin()));
        return _helpmenu;
}
