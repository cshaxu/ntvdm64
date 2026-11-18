/*++

Copyright (c) 1991-1992  Microsoft Corporation

Module Name:

    xactsrvp.h

Abstract:

    Private header file for XACTSRV.

Author:

    David Treadwell (davidtr) 05-Jan-1991

Revision History:

    02-Jun-1992 JohnRo
        RAID 9829: Avoid SERVICE_ equate conflicts.

--*/

#ifndef _XACTSRVP_
#define _XACTSRVP_

//
// To make netlib declare NetpDbgPrint.
//

#if DBG
#error Unsupported excluded original configuration
#endif
#line 36

//
// "System" include files
//

#include <stdio.h>
#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>

#include <ctype.h>
#include <stdlib.h>
#include <windef.h>
#include <winbase.h>
#include <string.h>
//!!UNICODE!! - Include tstring.h TSTR type-independent functions
#include <tstring.h>

//
// Network include files.
//

#include <nettypes.h>

#include <smbtypes.h>
#include <smbmacro.h>
#include <smbgtpt.h>

#line 68
#include <lm.h>         // LM20_SERVICE_ equates, etc.

#line 71
#include <winsvc.h>     // SERVICE_ equates, etc.

#include <apiparam.h>
#include <remdef.h>
#include <remtypes.h>
#include <netlib.h>
#include <netdebug.h>
#include <rap.h>

//
// Xactsrv's own include files
//

#line 85
#include <XsTypes.h>
#line 87
#include <XsConst.h>              // XactSrv.h must precede XsConst.h
#include <XsData.h>               // XsTypes.h must precede XsData.h
#include <XsUnicod.h>
#include <XsProcs.h>              // XsTypes.h must precede XsProcs.h.
#include <XsProcsP.h>             // XsTypes.h, XsConst.h and XsUnicod.h
                                  // must precede XsProcsP.h.
#include <XsDef16.h>
#include <XsParm16.h>

//
// !!! Temporary definitions for stubs.
//

#include <WkstaDef.h>

#ifndef STATIC
#define STATIC static
#endif
#ifndef IF_DEBUG
#define IF_DEBUG(_x) if (0)
#endif

#line 102
#endif // ndef _XACTSRVP_

