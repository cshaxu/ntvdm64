/*++ BUILD Version: 0001
 *
 *  WOW v1.0
 *
 *  Copyright (c) 1991, 1992, 1993 Microsoft Corporation
 *
 *  WOWTBL.H
 *  WOW32 API thunk table
 *
--*/



/* thunk table
 */
extern W32 aw32WOW[];


//
// the order of these must not change!  see kernel31\kdata.asm
//
typedef struct {
    WORD    kernel;
    WORD    dkernel;
    WORD    user;
    WORD    duser;
    WORD    gdi;
    WORD    dgdi;
    WORD    keyboard;
    WORD    sound;
    WORD    shell;
    WORD    winsock;
    WORD    toolhelp;
    WORD    mmedia;
    WORD    commdlg;
} TABLEOFFSETS;
typedef TABLEOFFSETS UNALIGNED *PTABLEOFFSETS;


VOID InitThunkTableOffsets(VOID);

extern TABLEOFFSETS tableoffsets;

#line 44
#ifdef DEBUG_OR_WOWPROFILE
#line 45
#error DIVERGENCE MVDM-HOST-DIV-587: WOW profiling declarations are not selected
#endif
#line 59
