/*++
 *
 *  WOW v1.0
 *
 *  Copyright (c) 1991, Microsoft Corporation
 *
 *  WMMSTRU1.C
 *  WOW32 16-bit MultiMedia structure conversion support
 *               Contains support for mciSendCommand Thunk message Parms.
 *               Also contains some debug support functions.
 *
 *  History:
 *  Created  14-Jul-1992 by Stephen Estrop (stephene)
 *
--*/

//
// We define NO_STRICT so that the compiler doesn't moan and groan when
// I use the FARPROC type for the Multi-Media api loading.
//
#define NO_STRICT


#include "precomp.h"
#pragma hdrstop

#if 0
#error DIVERGENCE MVDM-HOST-DIV-495: original inactive legacy multimedia branch excluded; see mirror register.
#endif
