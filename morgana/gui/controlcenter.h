/*#*********************************************************************
 ** ControlCenter
 **
 ** (c) Konrad Rosenbaum, 2000
 ** protected by the GNU GPL v.2 or any newer
 ***********************************************************************
 ** $Log: controlcenter.h,v $
 ** Revision 1.1  2000/09/21 18:40:52  pandur
 ** init
 **
 ***********************************************************************/
#ifndef M_CONTROLCENTER_H
#define M_CONTROLCENTER_H

#include <qmainwindow.h>

class MSession;
class IPackage;
class QTabWidget;
class QWidget;

/** 
  @short ControlCenter Class 

    This is the central controller for working networks. It is the starting
    point for all actions which manipulate one particular network as a whole.
    Eg.: Learning, Test, Evaluation
*/
class MControlCenter:public QMainWindow
{
        Q_OBJECT
        
        public:
          MControlCenter(MSession*);
          ~MControlCenter();
          
        private slots:
          void help();
          /**handles input from server*/
          void receive(IPackage&);
          void helpIndex();
          void helpManual();
          void aboutWin();
          /**opens a window for the Morgana-internal chat*/
          void chat();
          /**opens a 2D display window ( @ref SDisplay )*/
          void display();
          
        private:
          MSession*session;
          QTabWidget*tab;

          QPopupMenu*_filemenu;
          QPopupMenu*_patternmenu;
          QPopupMenu*_displaymenu;
          QPopupMenu*_bignetmenu;
          QPopupMenu*_netmenu;
          QPopupMenu*_helpmenu;
          
          void fillTabLearning(QWidget*);
          void fillTabPattern(QWidget*);
          void fillTabInit(QWidget*);
          void fillTabTest(QWidget*);
};





#endif //M_CONTROLCENTER_H
