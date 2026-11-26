/*++
 *
 *  WOW v1.0
 *
 *  Copyright (c) 1991, 1992, 1993 Microsoft Corporation
 *
 *  WOWTBL.C
 *  WOW32 API thunks
 *
 *  The reason for having one huge table is so the thunks can be dispatched
 *  faster.  When in separate tables, you had to do shifting and
 *  multiplication to derive the thunk routine from the function ID.
 *
 *
 *  History:
 *    barry bradie (barryb) 1-dec-92    combined individual tables
--*/
#include "precomp.h"
#pragma hdrstop
#ifdef FE_IME
#include "winnls32.h"
#include "wownls.h"
#include "wnman.h"
#endif // FE_IME
#ifdef FE_SB // suports WIFE API (MiscGetEUDCLeadByteRange)
#include "wowwife.h"
#include "wwmman.h"
#endif // FE_SB

#include "wowit.h"

MODNAME(wowtbl.c);

//
// DON'T CHANGE THE ORDER IN WHICH THESE FILES ARE INCLUDED!
//
// see W32GetTableOffsets (wow32.c) and kernel31\kdata.asm
//

W32 aw32WOW[] = {

#include "wktbl2.h"
    {W32FUN((LPFNW32)-1,                 "TABLESEPARATOR",            0,      0)},

#include "wutbl2.h"
    {W32FUN((LPFNW32)-1,                 "TABLESEPARATOR",            0,      0)},

#include "wgtbl2.h"
    {W32FUN((LPFNW32)-1,                 "TABLESEPARATOR",            0,      0)},

#include "wkbdtbl2.h"
    {W32FUN((LPFNW32)-1,                 "TABLESEPARATOR",            0,      0)},

#include "wstbl2.h"
    {W32FUN((LPFNW32)-1,                 "TABLESEPARATOR",            0,      0)},

#include "wshtbl2.h"
    {W32FUN((LPFNW32)-1,                 "TABLESEPARATOR",            0,      0)},

#include "wwstbl2.h"
    {W32FUN((LPFNW32)-1,                 "TABLESEPARATOR",            0,      0)},

#include "wthtbl2.h"
    {W32FUN((LPFNW32)-1,                 "TABLESEPARATOR",            0,      0)},

#include "wmmtbl2.h"
    {W32FUN((LPFNW32)-1,                 "TABLESEPARATOR",            0,      0)},

#include "wcmdgtbl.h"
    {W32FUN((LPFNW32)-1,                 "TABLESEPARATOR",            0,      0)},
#ifdef FE_SB
#ifdef FE_IME
#include "wntbl2.h"
    {W32FUN((LPFNW32)-1,                 "TABLESEPARATOR",            0,      0)},
#endif // FE_IME
#include "wwmtbl2.h"
    {W32FUN((LPFNW32)-1,                 "TABLESEPARATOR",            0,      0)},
#endif // !FE_SB
};


TABLEOFFSETS tableoffsets;

// REMOVECODE Remove comments below before shipping NT 5.  See Also WOW32Unimplemented95API in wow32.c and wowtbl.h
// #ifdef DEBUG_OR_WOWPROFILE
INT cAPIThunks;
// #endif

#ifdef WOWPROFILE
#error Excluded unsupported WOW profiling configuration
#endif
#line 92


VOID InitThunkTableOffsets(VOID)
{
    WORD    current;
    WORD    offsetarray[(MOD_LAST - MOD_KERNEL) / FUN_MASK + 1];
    UINT    i;

    for (current = 0; current < sizeof(aw32WOW)/sizeof(aw32WOW[0]); current++) {
         if (current == 0) {
             i = 0;
             offsetarray[i++] = current;
         }
         else if (aw32WOW[current].lpfnW32 == (LPFNW32)-1) {
             offsetarray[i++] = current + 1;
         }

    }
    tableoffsets.kernel  =
    tableoffsets.dkernel = offsetarray[MOD_KERNEL / FUN_MASK];
    tableoffsets.user =
    tableoffsets.duser = offsetarray[MOD_USER / FUN_MASK];
    tableoffsets.gdi =
    tableoffsets.dgdi = offsetarray[MOD_GDI / FUN_MASK];
    tableoffsets.keyboard = offsetarray[MOD_KEYBOARD / FUN_MASK];
    tableoffsets.sound = offsetarray[MOD_SOUND / FUN_MASK];
    tableoffsets.shell = offsetarray[MOD_SHELL / FUN_MASK];
    tableoffsets.winsock = offsetarray[MOD_WINSOCK / FUN_MASK];
    tableoffsets.toolhelp = offsetarray[MOD_TOOLHELP / FUN_MASK];
    tableoffsets.mmedia = offsetarray[MOD_MMEDIA / FUN_MASK];
    tableoffsets.commdlg = offsetarray[MOD_COMMDLG / FUN_MASK];
#ifdef FE_IME
    tableoffsets.winnls = offsetarray[MOD_WINNLS / FUN_MASK];
#endif // FE_IME
#ifdef FE_SB
    tableoffsets.wifeman = offsetarray[MOD_WIFEMAN / FUN_MASK];
#endif // FE_SB

#ifdef DEBUG_OR_WOWPROFILE
#error Excluded unsupported WOW profiling configuration
#endif
#line 133

}




#ifdef DEBUG_OR_WOWPROFILE


#error Excluded unsupported WOW profiling configuration
#endif
#line 313
