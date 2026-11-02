/*++ BUILD Version: 0001
 *
 *  WOW v1.0
 *
 *  Copyright (c) 1991, Microsoft Corporation
 *
 *  WMTBL32.H
 *  WOW32 32-bit Message Thunkers
 *
 *  History:
 *  Created 23-Feb-1992 by Chandan Chauhan (ChandanC)
--*/


#define THUNKMSG    1
#define UNTHUNKMSG  0


/* Message dispatch table
 */

extern M32 aw32Msg[];


#ifdef DEBUG_OR_WOWPROFILE
#error DIVERGENCE MVDM-HOST-DIV-587: WOW profiling declarations are not selected
#endif


#ifdef DEBUG
#error DIVERGENCE MVDM-HOST-DIV-596: DEBUG header profile is not selected
#else
#line 33
#define WM32UNDOCUMENTED WM32NoThunking
#endif
#line 35
