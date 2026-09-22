/* DIVERGENCE(MVDM-HOST-DIV-294): explicit cdecl for the existing CPU40
 * worker exports consumed by the original /Gz WOW32 DLL. Parameter types,
 * implementation and ownership are unchanged; no stack-incompatible aliases. */
/*++ BUILD Version: 0001
 *
 *  MVDM v1.0
 *
 *  Copyright (c) 1991,1992 Microsoft Corporation
 *
 *  DBGEXP.H
 *  DBG exports
 *
 *  History:
 *  13-Jan-1992 Bob Day (bobday)
 *  Created.
--*/

extern BOOL DBGInit( int, char *[] );
extern VOID DBGDispatch( void );
extern VOID __cdecl DBGNotifyNewTask( LPVOID pNTFrame, UINT uFrameSize );
extern VOID __cdecl DBGNotifyRemoteThreadAddress( LPVOID lpAddress, DWORD lpBlock );
extern VOID __cdecl DBGNotifyDebugged( BOOL fDebugged );
