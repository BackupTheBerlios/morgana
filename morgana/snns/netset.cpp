/*#**********************************************************************
 ** Creation, Modification and Deletion of Units, and SubNets
 ************************************************************************
 ** $Log: netset.cpp,v $
 ** Revision 1.1  2000/09/21 18:40:56  pandur
 ** init
 **
 **
 ************************************************************************/
#include "snns.h"

#include "main.h"

#include "debug.h"

#include <internal/types.h>
#include <ipc/connection.h>
#include <ipc/package.h>


#include<qstring.h>

#if __GNUC__ == 2
#pragma implementation "unit.h"
#endif
#include "unit.h"

static inline void creatUnit(IPackage&p,IConnection*c)
{
        uint32 subnet;
        uint16 numunits;
        SUnit unit,ounit;
        p>>subnet>>numunits;
        IPackage op("set");
        op<<QString("create unit");
        op<<subnet<<numunits;
        int num,err;
        //yet no check for subnet/layer defined, SNNS itself gives damn
        //buffer for string ops:
        char buf[256];
        //create units
        for(int i=0;i<numunits;i++){
                getUnit(unit,p);
                ounit=unit;
                num=krui_createDefaultUnit();
                if(num<0){//error
                        ounit.mask=UERR;
                        ounit.errcode=(QString)krui_error(-num);
                        setUnit(ounit,op);
                        //proceed with next unit
                        continue;
                }else{//success
                        ounit.mask|=UNUM;
                        ounit.id=num;
                }
                //subnet/layer
                krui_setUnitLayerNo(num,subnet&0xffff);
                int subn=(subnet>>16)&0xffff;
                subn-=32736;
                krui_setUnitSubnetNo(num,subn);
                //name
                if(unit.mask&UNAME){
                        err=krui_setUnitName(num,CHR(buf,unit.name));
                        if(err){
                                ounit.mask|=UERR;
                                ounit.errcode=krui_error(err);
                        }
                }
                //output function
                if(unit.mask&UOUTF){
                        err=krui_setUnitOutFunc(num,CHR(buf,unit.outfunc));
                        if(err){
                                ounit.mask|=UERR;
                                ounit.errcode=krui_error(err);
                        }
                }//must be for error and nomask, doesn't hurt anyway
                ounit.mask|=UOUTF;
                ounit.outfunc=krui_getUnitOutFuncName(num);
                //activation function
                if(unit.mask&UACTF){
                        err=krui_setUnitActFunc(num,CHR(buf,unit.actfunc));
                        if(err){
                                ounit.mask|=UERR;
                                ounit.errcode=krui_error(err);
                        }
                }
                ounit.mask|=UACTF;
                ounit.actfunc=krui_getUnitActFuncName(num);
                //initial activation
                if(unit.mask&UIACT)
                        krui_setUnitInitialActivation(num,unit.iact);
                //activation
                if(unit.mask&UACT)
                        krui_setUnitActivation(num,unit.act);
                //bias
                if(unit.mask&UBIAS)
                        krui_setUnitBias(num,unit.bias);
                //prototype
                if(unit.mask&UPROTO){
                        err=krui_setUnitFType(num,CHR(buf,unit.prototype));
                        if(err){
                                ounit.mask|=UERR;
                                ounit.errcode=krui_error(err);
                                ounit.prototype=(char*)0;
                                //take this as non-fatal and proceed
                        }
                }
                //position
                if(unit.mask&UPOS){
                        struct PosType pos;
                        pos.x=unit.xpos;
                        pos.y=unit.ypos;
                        pos.z=unit.zpos;
                        krui_setUnitPosition(num,&pos);
                        //the above doesn't transport errors, so ...
                        krui_getUnitPosition(num,&pos);
                        ounit.xpos=pos.x;
                        ounit.ypos=pos.y;
                        ounit.zpos=pos.z;
                }else {
                        struct PosType pos;
                        ounit.mask|=UPOS;
                        krui_getUnitPosition(num,&pos);
                        ounit.xpos=pos.x;
                        ounit.ypos=pos.y;
                        ounit.zpos=pos.z;
                }
                //send back
                setUnit(ounit,op);
        }
        c->send(op);
}

static inline void modifUnit(IPackage&p,IConnection*c)
{
        uint32 subnet;
        uint16 numunits;
        int err;
        SUnit unit,ounit;
        IPackage op("set");
        op<<QString("modify unit");
        op<<subnet<<numunits;
        char buf[256];
        for(int i=0;i<numunits;i++){
                getUnit(unit,p);
                ounit=unit;
                //check for existance
                if(!(unit.mask&UNUM)){
                      int num=krui_createDefaultUnit();
                      if(num<0){
                              ounit.errcode="Unable to create unit.";
                              setUnit(ounit,op);
                              continue;
                      }
                      unit.id=ounit.id=num;
                }
                if(unit.id==0||unit.id>krui_getNoOfUnits()){
                        ounit.mask=UERR|UNUM;
                        ounit.errcode="Unit doesn't exist.";
                        setUnit(ounit,op);
                        continue;
                }
                //prototype
                if(unit.mask&UPROTO){
                        err=krui_setUnitFType(unit.id,CHR(buf,unit.prototype));
                        if(err){
                                ounit.prototype=krui_getUnitFTypeName(unit.id);
                                ounit.mask|=UERR;
                                ounit.errcode=krui_error(err);
                        }
                }
                //position
                if(unit.mask&UPOS){
                        PosType pos;
                        pos.x=unit.xpos;
                        pos.y=unit.ypos;
                        pos.z=unit.zpos;
                        krui_setUnitPosition(unit.id,&pos);
                        krui_getUnitPosition(unit.id,&pos);
                        ounit.xpos=pos.x;
                        ounit.ypos=pos.y;
                        ounit.zpos=pos.z;
                }
                //bias
                if(unit.mask&UBIAS){
                        krui_setUnitBias(unit.id,unit.bias);
                }
                //i_activation
                if(unit.mask&UIACT){
                        krui_setUnitInitialActivation(unit.id,unit.iact);
                }
                //activation
                if(unit.mask&UACT){
                        err=krui_setUnitActivation(unit.id,unit.act);
                        if(err){
                                ounit.act=krui_getUnitActivation(unit.id);
                                ounit.mask|=UERR;
                                ounit.errcode=krui_error(err);
                        }
                }
                //actfunc
                if(unit.mask&UACTF){
                        err=krui_setUnitActFunc(unit.id,CHR(buf,unit.actfunc));
                        if(err){
                                ounit.mask|=UERR;
                                ounit.errcode=krui_error(err);
                                ounit.actfunc=krui_getUnitActFuncName(unit.id);
                        }
                }
                //outfunc
                if(unit.mask&UOUTF){
                        err=krui_setUnitOutFunc(unit.id,CHR(buf,unit.outfunc));
                        if(err){
                                ounit.mask|=UERR;
                                ounit.errcode=krui_error(err);
                                ounit.outfunc=krui_getUnitOutFuncName(unit.id);
                        }
                }
                //name
                if(unit.mask&UNAME){
                        err=krui_setUnitName(unit.id,CHR(buf,unit.name));
                        if(err){
                                ounit.mask|=UERR;
                                ounit.errcode=krui_error(err);
                                ounit.name=krui_getUnitName(unit.id);
                        }
                }
                //store
                setUnit(ounit,op);
        }
        c->send(op);
}


static inline void delUnit(IPackage&p,IConnection*c)
{
        uint32 subnet,unit,max;
        uint16 numunits;
        p>>subnet>>numunits;
        if(numunits<1)return;
        int uarr[numunits],num=0,i;
        max=krui_getNoOfUnits();
        for(i=0;i<numunits;i++){
                p>>unit;
                if(unit<=max&&unit!=0)
                        uarr[num++]=unit;
        }
        if(num==0)return;
        int err=krui_deleteUnitList(num,uarr);
        //create deletion package
        IPackage op("set");
        op<<QString("delete unit")<<subnet<<(uint16)num;
        for(i=0;i<num;i++)op<<(uint32)(uarr[i]);
        c->send(op);
        //send workaround packages
        for(i=0;i<num;i++)sendSNNS(c,"delete unit",uarr[i]-i);
}



static inline void creatNet(IPackage&p,IConnection*c)
{
}

static inline void delNet(IPackage&p,IConnection*c)
{
}

static inline void creatSite(IPackage&p,IConnection*c)
{
}

static inline void modifSite(IPackage&p,IConnection*c)
{
}

static inline void delSite(IPackage&p,IConnection*c)
{
}

static inline void creatCon(IPackage&p,IConnection*c)
{
}

static inline void delCon(IPackage&p,IConnection*c)
{
}


////////////////////////////////////////////////////////////////////

void SMain::reqSet(IPackage&p,IConnection*c)
{
        p.pos(0);
        QString s;
        p>>s;
        if(s=="create unit")creatUnit(p,c);else
        if(s=="modify unit")modifUnit(p,c);else
        if(s=="delete unit")delUnit(p,c);else
        if(s=="create site")creatSite(p,c);else
        if(s=="modify site")modifSite(p,c);else
        if(s=="delete site")delSite(p,c);else
        if(s=="create connection")creatCon(p,c);else
        if(s=="delete connection")delCon(p,c);else
        if(s=="create subnet")creatNet(p,c);else
        if(s=="delete subnet")delNet(p,c);
#ifdef MDEBUG
        else mDebug("SNNS: set-handler: unknown command \"%s\"\n",
                (const char*)s);
#endif //MDEBUG
}
