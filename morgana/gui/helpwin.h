/*#*********************************************************************
 ** Help Browser/Window
 **
 ** (c) Konrad Rosenbaum, 2000
 ** protected by the GNU GPL v.2 or any newer
 ***********************************************************************
 ** $Log: helpwin.h,v $
 ** Revision 1.1  2000/09/21 18:40:52  pandur
 ** init
 **
 ***********************************************************************/

#ifndef M_HELPWIN_H
#define M_HELPWIN_H

#include <qdialog.h>
#include <qdict.h>
#include <qstring.h>

class QTextBrowser;

/**
  @short Displays help texts which are selected by a file name
*/
class MHelpWin:public QWidget
{
        Q_OBJECT
        
        public:
          /**opens a help window with file in the display*/
          MHelpWin(QString file);
          ~MHelpWin();
          
        private slots:
          void textChanged();
          void index();
          void about();
          
        private:
          QTextBrowser*tb;
        
          /**
          @internal
          stores the directory where the texts are found*/
          static QString directory;
          /**
          @internal
          used to translate HTML-symbols into latin-1*/
          static QDict<QString> dict;
          
          static void filldict();
          static void finddir();
          
          /**pointer to the currently open help-window to prevent
          hundreds of open help-windows*/
          static MHelpWin*current;
          MHelpWin*previous,*next; 
          
        public:
          /**opens file and either displays it in the current help window
          or opens a new one*/
          static void contextHelp(QString file);
};



#endif //M_HELPWIN_H
