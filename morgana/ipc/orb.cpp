/*#*************************************************************************
 ** local ORB $Revision: 1.2 $
 ***************************************************************************
 ** (c) Konrad Rosenbaum, 2000
 ** protected by the GNU GPL version 2 or any newer
 ***************************************************************************
 ** History:
 **
 ** $Log: orb.cpp,v $
 ** Revision 1.2  2001/08/30 18:04:59  pandur
 ** *** empty log message ***
 **
 ** Revision 1.1  2000/11/25 15:23:35  pandur
 ** orb added, some new code
 **
 ***************************************************************************/


#if __GNUC__ == 2
#pragma implementation "ipc/orb.h"
#endif

#include "orb.h"
#warning IRemote needs error handling for alloc, to be done in: orb.h


IORB *IORB::orb=0;
QDict<void> *IORB::loo=0,*IORB::lom=0;


IORB::IORB()
{
        isstatic=false;
        ginit();
}

void IORB::ginit()
{
        if(!orb)orb=new IORB(true);
}

IORB::IORB(bool)
{
        isstatic=true;
        loo=new QDict<void>;
        lom=new QDict<void>;
}

bool IORB::registerMethod(const QString&rmeth,QObject*lobj,char*method)
{
        ginit();
}

bool IORB::unregisterMethod(const QString&rmeth)
{
        ginit();
}

bool IORB::registerObject(const QString&robj,QObject*obj,bool allowothers)
{
        ginit();
}


