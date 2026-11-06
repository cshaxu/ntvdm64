/*++

Copyright (c) 1991  Microsoft Corporation

Module Name:

    vrica.h

Abstract:

    Defines exported function in ntvdm.exe which calls the ica_hw_interrupt
    function

    Contents:

Author:

    Richard L Firth (rfirth) 09-Jun-1992

Revision History:

--*/

/* DIVERGENCE(MVDM-HOST-DIV-884): the public interrupt export is stdcall;
 * retain that ABI in this cdecl CPU40 redirector build. */
extern void __stdcall call_ica_hw_interrupt(int, int, int);
