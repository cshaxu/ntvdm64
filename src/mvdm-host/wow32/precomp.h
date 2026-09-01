/*++ BUILD Version: 0001
 *
 *  Copyright (c) 1991, Microsoft Corporation
 *
 *  precomp.h
 *  Combined precompiled header source
 *
 *  This file is a collection of all the common .h files used by the
 *  various source files in this directory. It is precompiled by the
 *  build process to speed up the overall build time.
 *
 *  Put new .h files in here if it has to be seen by multiple source files.
 *  Keep in mind that the definitions in these .h files are potentially
 *  visible to all source files in this project.
 *
 *  History:
 *  Created 19-Oct-1993 by Neil Sandlin (neilsa)
--*/

#include <stddef.h>
/* DIVERGENCE(MVDM-HOST-DIV-180): modern `windows.h` includes its incompatible
 * post-NT4 WOW export declarations. The original WOW32 body remains the
 * selected provider, so keep that unrelated SDK declaration carrier out. */
#ifndef _WOWNT32_
#define _WOWNT32_
#endif
#include <nt.h>
/* DIVERGENCE(MVDM-HOST-DIV-179): the original build's private include-root
 * made these declaration carriers implicit.  The split mirror uses the
 * byte-identical opennt-abi carriers; this selects no USER/GDI behavior. */
#include <winuserp.h>
/* DIVERGENCE(MVDM-HOST-DIV-181): the SDK's host-architecture `_X86_` marker
 * selects a native-VDM register alias in original WOW32. The selected
 * CPU40 SoftPC path is emulated on both host widths and must retain the
 * original simulator `getSS`/`setSS` stack form instead. */
#ifdef _X86_
#undef _X86_
#endif
#include "wow32.h"
#include "wowtbl.h"
#include "doswow.h"
#include "wdos.h"
#include "wmdisp32.h"
#include "mapembed.h"
#include "wowusr.h"
#include "waccel.h"
#include "wcall16.h"
#include "wuclass.h"
#include "wsubcls.h"
#include <mmsystem.h>
#include "wkman.h"
#include "fastwow.h"
#include "wcall32.h"
#include "wudlg.h"
#include "tdb16.h"
#include "wcntl32.h"
#include "wcuricon.h"
#include "wmsg16.h"
#include "wmsgbm.h"
#include "wmtbl32.h"
#include "wcommdlg.h"
#include "wowcmdlg.h"
#include <commdlg.h>
#include "wres16.h"
#include "wres32.h"
#include "wowkrn.h"
#include "wdde.h"
#include <dde.h>
#include "wuclip.h"
#include "wgmeta.h"
#include "wowgdi.h"
#include "wgdi.h"
#include "wgprnset.h"
#include "wgfont.h"
#include "wgdi31.h"
#include "wgman.h"
#include "wgpal.h"
#include "wgtext.h"
#include "wheap.h"
#include "wowkbd.h"
#include "wkbman.h"
#include "wkernel.h"
#include "wkfileio.h"
#include <winbase.h>
#include "oemuni.h"
#include "vrnmpipe.h"
#include "wkgthunk.h"
#include "wklocal.h"
#include "wowhooks.h"
#include "wutmr.h"
#include "wreldc.h"
#include "vdmapi.h"
#include "wowinfo.h"
#include "dbgexp.h"
#include "wucomm.h"
#include "wowmmcb.h"
#include "isz.h"
#include "wkmem.h"
#include <mmddk.h>
#include "wowmmed.h"
#include "wmmstruc.h"
#include "wmmedia.h"
#include "isvwow.h"
#include <string.h>
#include <digitalv.h>
#include "wmsgcb.h"
#include "wmsgem.h"
#include "wmsglb.h"
#include "wmsgsbm.h"
#include "wumsg.h"
#include "wuman.h"
#include <vdm.h>
#include "wucaret.h"
#include "wucursor.h"
#include "wuhook.h"
#include "wumenu.h"
#include "wuser.h"
#include "wutext.h"
#include "wuwind.h"
#include "wuser31.h"
#include "wulang.h"
#include "winsockp.h"
#include "wowsnd.h"
#include "wsman.h"
#include "wowshell.h"
#include "wshell.h"
#include "wowth.h"
#include "wthman.h"
#include "wusercli.h"
#include "wole2.h"
#include "wparam.h"
#include <limits.h>

