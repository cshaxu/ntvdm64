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

/* DIVERGENCE(HOST-DIV-017): OpenNT exports this function from a separately
 * loaded VDMREDIR DLL. The statically composed importer retains the original
 * function-pointer ABI, so only the emitted provider symbol is privately
 * renamed; the retained source spelling below is unchanged. */
#define VrInitialized bx_ntvdm_vr_initialized_provider

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
