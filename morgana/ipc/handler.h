/*#*************************************************************************
 ** Abstract Handler for incoming/outgoing traffic $Revision: 1.1 $
 ***************************************************************************
 ** (c) Konrad Rosenbaum, 2000
 ** protected by the GNU GPL version 2 or any newer
 ***************************************************************************
 ** History:
 **
 ** $Log: handler.h,v $
 ** Revision 1.1  2000/09/24 16:22:24  pandur
 ** *** empty log message ***
 **
 **
 ***************************************************************************/


#include <qobject.h>

class IHandler:public QObject
{
        Q_OBJECT
        
        public slots:
          virtual receive(uint16 channel,const void*buf,int size)=0;
          
        signals:
          virtual send(uint16 channel,const void*buf,int size)=0;
};
