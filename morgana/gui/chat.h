/*#*********************************************************************
 ** Chat: IPC-Test
 **
 ** (c) Konrad Rosenbaum, 2000
 ** protected by the GNU GPL v.2 or any newer
 ***********************************************************************
 ** $Log: chat.h,v $
 ** Revision 1.1  2000/09/21 18:40:52  pandur
 ** init
 **
 ***********************************************************************/
#ifndef M_CHAT_H
#define M_CHAT_H

#include <qobject.h>
#include <qvbox.h>

class MSession;
class IPackage;
class QTextView;
class QLineEdit;

/**
  @short simple Chat window
  
  This is just a small class to test how the IPC works.
*/
class MChat:public QVBox{
        Q_OBJECT
        
        public:
          MChat(MSession*);
          ~MChat();
         
        private slots:
          void receive(IPackage&);
          void send();
          
        private:
          MSession*session;
          QTextView*viewer;
          QLineEdit*ident;
          QLineEdit*input;
};

#endif //M_CHAT_H
