/*++

Copyright (c) 1991  Microsoft Corporation

Module Name:

    vrinit.c

Abstract:

    Contains Vdm Redir (Vr) 32-bit side initialization and uninitialization
    routines

    Contents:
        VrInitialized

Author:

    Richard L Firth (rfirth) 13-Sep-1991

Environment:

    32-bit flat address space

Revision History:

    13-Sep-1991 RFirth
        Created

--*/

#include <windows.h>

/* DIVERGENCE(HOST-DIV-017): private static-provider link binding. */
#include "opennt-host-overlay/vdmredir/vrinit_bindings.h"
/* DIVERGENCE(HOST-DIV-022): this reached provider-only subset crops the
 * original VrInitialize/VrUninitialize, interrupt/async-completion and
 * suspend/resume-hook NT4 VDD/NetBIOS/DLC/ICA product shell.  No retained
 * provider calls those independent routines; their 57:xx admission remains
 * explicitly declined by the OpenNT-BOP owner. */

//
// data
//

static BOOLEAN IsVrInitialized = FALSE; // set when TSR loaded


//
// routines
//

BOOLEAN
VrInitialized(
    VOID
    )

/*++

Routine Description:

    Returns whether the VdmRedir support has been initialized yet (ie redir.exe
    TSR loaded in DOS emulation memory). Principally here because VdmRedir is
    now a DLL loaded at run-time via LoadLibrary

Arguments:

    None.

Return Value:

    BOOLEAN
        TRUE    VdmRedir support is active
        FALSE   VdmRedir support inactive

--*/

{
    return IsVrInitialized;
}
