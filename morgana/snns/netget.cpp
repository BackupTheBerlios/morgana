/*#*********************************************************************
 ** GET methods
 ***********************************************************************
 ** $Log: netget.cpp,v $
 ** Revision 1.1  2000/09/21 18:40:56  pandur
 ** init
 **
 **
 ***********************************************************************/

#include "snns.h"

#include "main.h"

#include "debug.h"

#include <internal/types.h>
#include <ipc/connection.h>
#include <ipc/package.h>

#include "unit.h"

#include<qstring.h>

////////////////////////////////////////////////////////////////////////
// statics

static void getUnit(IPackage&p,IConnection*c)
{
        uint32 sub,num;
        p>>sub;
        p>>num;
        if(num==0 || num>krui_getNoOfUnits()){
                //unit doesn't exist: inform client to delete it
                IPackage op("set");
                op<<"delete unit"<<sub<<(uint16)1<<num;
                c->send(op);
                return;
        }
        IPackage op("get");
        op<<QString("unit")<<sub<<(uint16)1;
        SUnit u;
        u.mask=UNUM|UPOS|UBIAS|UIACT|UACT|UACTF|UOUTF|UNAME;
        u.id=num;
        struct PosType pos;
        krui_getUnitPosition(num,&pos);
        u.xpos=pos.x;
        u.ypos=pos.y;
        u.zpos=pos.z;
        u.iact=krui_getUnitInitialActivation(num);
        u.act=krui_getUnitActivation(num);
        u.actfunc=krui_getUnitActFuncName(num);
        u.outfunc=krui_getUnitOutFuncName(num);
        u.name=krui_getUnitName(num);
        setUnit(u,op);
        c->send(op);
}

static void getNet(IPackage&p,IConnection*c)
{
}

static void getFull(IPackage&p,IConnection*c)
{
}

static void getArea(IPackage&p,IConnection*c)
{
}

static void getNumbers(IPackage&p,IConnection*c)
{
        IPackage op("get");
        op<<QString("numbers")<<(uint32)0<<(uint32)krui_getNoOfUnits();
        c->send(op);
}


////////////////////////////////////////////////////////////////////////
// request


void SMain::reqGet(IPackage&p,IConnection*c)
{
        p.pos(0);
        QString s;
        p>>s;
        
        if(s=="unit")getUnit(p,c);else
        if(s=="network")getNet(p,c);else
        if(s=="full")getFull(p,c);else
        if(s=="area")getArea(p,c);else
        if(s=="numbers")getNumbers(p,c);
#ifdef DEBUG
        else mDebug("SNNS: got unknow get-package: %s\n",(const char*)s);
#endif
}
