/* ************************************************************
 ** just includes all Headers needed to compile against
 ** the SNNS kernel
 **************************************************************
 ** $Log: snns.h,v $
 ** Revision 1.1  2000/09/21 18:40:56  pandur
 ** init
 **
 **
 **************************************************************/

#ifndef S_SNNS_H
#define S_SNNS_H
extern "C" {

//for compatibility with C+SNNS:
#define bool snnsbool

#include "../../SNNS/kernel/sources/glob_typ.h"
#include "../../SNNS/kernel/sources/ext_typ.h"
#include "../../SNNS/kernel/sources/art_typ.h"
#include "../../SNNS/kernel/sources/func_mac.h"

#include "../../SNNS/kernel/sources/krart_df.h"
#include "../../SNNS/kernel/sources/krui_typ.h"
#include "../../SNNS/kernel/sources/kr_const.h"
#include "../../SNNS/kernel/sources/kr_def.h"
#include "../../SNNS/kernel/sources/kr_mac.h"
#include "../../SNNS/kernel/sources/kr_typ.h"
#include "../../SNNS/kernel/sources/kr_ui.h"
#include "../../SNNS/kernel/sources/version.h"

#undef bool

} //end of extern "C"

#endif //S_SNNS_H
