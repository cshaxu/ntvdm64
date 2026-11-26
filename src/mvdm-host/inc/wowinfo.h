/*++ BUILD Version: 0002
 *
 *  WOW v1.0
 *
 *  Copyright (c) 1991, Microsoft Corporation
 *
 *  WOWINFO.H
 *  16-bit Kernel API argument structures
 *
 *  History:
 *  Created 1-jun-1992 by Matt Felton (mattfe)
--*/

// the following UNALIGNED definition is required because wowinfo.h is
// included in 'wowexec.c'
//
// these lines are from ntdef.h
//

#ifndef UNALIGNED
#error T476-S1: branch not selected by product build
#endif
#line 29

/* XLATOFF */
#pragma pack(2)
/* XLATON */

typedef struct _WOWINFO {               /**/
    LPSTR       lpCmdLine;
    LPSTR       lpAppName;
    LPSTR       lpEnv;
    DWORD       iTask;
    USHORT      CmdLineSize;
    USHORT      AppNameSize;
    USHORT      EnvSize;
    USHORT      CurDrive;
    LPSTR       lpCurDir;
    USHORT      CurDirSize;
    USHORT      wShowWindow;
} WOWINFO;
typedef WOWINFO UNALIGNED *PWOWINFO;

#define MAXENVIRONMENTSIZE	2048	// Max Size of Environment coped with.


/* XLATOFF */
#pragma pack()
/* XLATON */

#define WM_WOWEXECSTARTAPP         (WM_USER)    // also in windows\inc\vdmapi.h
#define WM_WOWEXECHEARTBEAT        (WM_USER+1)  // To deliver timer ticks
#define WM_WOWEXEC_START_TASK      (WM_USER+2)  // vdmdbg.dll sends this
#define WM_WOWEXECSTARTTIMER       (WM_USER+3)  // see WK32WowShutdownTimer
