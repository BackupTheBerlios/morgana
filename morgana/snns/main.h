#ifndef S_MAIN_H
#define S_MAIN_H

#include <qobject.h>
#include <qptrdict.h>
#include <qstringlist.h>

class IPackage;
class IListener;
class IConnection;
class ISched;

/**
 @short Main class of the Morgana server side in the SNNS dialect
 
 Since SNNS came out of a Non-OO design this is the only real class in
 this implementation of the server. It simulates more than one class to the
 client side. This decision wasn't made because it would define a good design
 (in fact I believe it's one of the worst), but to provide as much efficiency
 as possible in respect to runtime and memory usage. To create more classes
 would waste twice as much memory as the SNNS-kernel uses itself.
 
 The implementation of this class is spread over several files, each 
 containing one large chunk of functionality. main.cpp contains
 all central functionality, like connection establishment, logins and
 signal handling. Please refer to the comments in the various files for
 details on them.
 
 This server, and though this class, will be replaced by an own variant of
 Neuro-Implementation which will have a design which will be well-thought-out,
 efficient and in a good and modern OO-design. (Hopefully)
 */
class SMain:QObject
{
        Q_OBJECT
        
        public:
          //*constructs and initializes the server
          SMain();
          //*shuts everything down
          ~SMain();
          
        private:
          //*access control list for all connections
          QPtrDict<QStringList> conACL;
          QPtrDict<QStringList> conFlags;
          
          /**main.cpp:" signal handling": reacts on short signals from 
          autorized clients; for example "terminate", which causes the server to
          shut down*/
          void reqSignal(IPackage&,IConnection*);
          //*main.cpp: handles requests like "who are you?", "may I connect?"
          void reqRequest(IPackage&,IConnection*);
          //*main.cpp: Login/Logoff processing
          void reqLogin(IPackage&,IConnection*);
          //*chat.cpp: handles chat packages
          void reqChat(IPackage&,IConnection*);
          //*netset.cpp: set packages, creates/modifies nets
          void reqSet(IPackage&,IConnection*);
          //*netget.cpp: get packages, retrieve network data
          void reqGet(IPackage&,IConnection*);
          
        private slots:
          /**gets all packages, checks permission, and spreads them into the
          various req** methods*/
          void request(IPackage&,IConnection*);
          //*enables a new connection to send packages
          void newConnection(IConnection*);
          //*shuts a connection down
          void termConnection(IConnection*);
};


#endif //S_MAIN_H
