/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    dpmi32.c

Abstract:

    This function contains common code such as the dpmi dispatcher,
    and handling for the initialization of the dos extender.

Author:

    Dave Hastings (daveh) 24-Nov-1992

Revision History:

    Neil Sandlin (neilsa) 31-Jul-1995 - Updates for the 486 emulator

--*/
/* DIVERGENCE (T257 S3): the NT4 `precomp.h`/`softpc.h` product shell exposes
 * VdmTib, Sim32GetVDMPointer and a process LDT.  The named shim retains only
 * the original startup/table calls through fixed-width session records. */
#include "../../../adapter-softpc/dpmi_startup_source_shim.h"
//
// Information about the current PSP
//
USHORT CurrentPSPSelector;

//
// Table of selector bases and limits
//
ULONG FlatAddress[LDT_SIZE];

//
// Index # for DPMI bop.  Used for error reporting on risc
//
ULONG Index;

//
// DPMI dispatch table
//
VOID (*DpmiDispatchTable[MAX_DPMI_BOP_FUNC])(VOID) = {
    DpmiSetDescriptorEntry,                     // 0
    switch_to_protected_mode,                   // 1
    DpmiSetProtectedmodeInterrupt,              // 2
    DpmiGetFastBopEntry,                        // 3
    DpmiInitDosx,                               // 4
    DpmiInitApp,                                // 5
    DpmiXlatInt21Call,                          // 6
    DpmiAllocateXmem,                           // 7
    DpmiFreeXmem,                               // 8
    DpmiReallocateXmem,                         // 9
    DpmiSetFaultHandler,                        // a
    DpmiGetMemoryInfo,                          // b
    DpmiDpmiInUse,                              // c
    DpmiDpmiNoLongerInUse,                      // d
    DpmiSetDebugRegisters,                      // e
    DpmiPassTableAddress,                       // f
    DpmiFreeAppXmem,                            // 10
    DpmiPassPmStackInfo,                        // 11
    DpmiVcdPmSvcCall32,                         // 12
    DpmiFreeAllXmem,                            // 13
    DpmiIntHandlerIret16,                       // 14
    DpmiIntHandlerIret32,                       // 15
    DpmiFaultHandlerIret16,                     // 16
    DpmiFaultHandlerIret32,                     // 17
    DpmiUnhandledExceptionHandler               // 18
};

VOID
DpmiDispatch(
    VOID
    )
/*++

Routine Description:

    This function dispatches to the appropriate subfunction

Arguments:

    None

Return Value:

    None.

--*/
{

    /* DIVERGENCE (T257 S3): the generic BOP ingress is not admitted here.
     * A later bounded ingress stages the already-copied selector byte; this
     * keeps the original table dispatch/one-byte IP advance ordering without
     * exporting an NT4 guest pointer. */
    if (!bx_ntvdm_dpmi_startup_source_take_dispatch(&Index)) {
        return;
    }
    bx_ntvdm_dpmi_startup_source_advance_ip(1u); // take care of subfn.

    DBGTRACE(DPMI_DISPATCH_ENTRY, Index, 0, 0);

    if (Index >= MAX_DPMI_BOP_FUNC) {
#if DBG
        DbgPrint("NtVdm: Invalid DPMI BOP %lx\n", Index);
#endif
        return;
    }

    (*DpmiDispatchTable[Index])();
}

VOID
DpmiIllegalFunction(
    VOID
    )
/*++

Routine Description:

    This routine ignores any Dpmi bops that are not implemented on a
    particular platform. It is called through the DpmiDispatchTable
    by #define'ing individual entries to this function.
    See dpmidata.h and dpmidatr.h.

Arguments:

    None.

Return Value:

    None.

--*/
{
   /* DIVERGENCE (T257 S3): retain the table's invalid-service terminal but
    * do not import NT4 debugger/window output plumbing. */
   bx_ntvdm_dpmi_startup_source_note_illegal(Index);
}

VOID
DpmiInitDosx(
    VOID
    )
/*++

Routine Description:

    This routine handle the initialization bop for the dos extender.
    It get the addresses of the structures that the dos extender and
    32 bit code share.

Arguments:

    None

Return Value:

    None.

--*/
{
    /* DIVERGENCE (T257 S3): original field order is retained by the named
     * session seam, but pointer-valued Sim32GetVDMPointer results become
     * checked fixed-width guest-linear values. */
    bx_ntvdm_dpmi_startup_source_initialize_dosx();

}

VOID
DpmiInitApp(
    VOID
    )
/*++

Routine Description:

    This routine handles any necessary 32 bit initialization for extended
    applications.

Arguments:

    None.

Return Value:

    None.

Notes:

    This function contains a number of 386 specific things.
    Since we are likely to expand the 32 bit portions of DPMI in the
    future, this makes more sense than duplicating the common portions
    another file.

--*/
{
    /* DIVERGENCE (T257 S3): preserve `AX & DPMI_32BIT` and startup frame
     * record ordering through the staged copied CPU/frame request.  NT4's
     * VdmTib/SS:SP pointer translation is unavailable and is not recreated. */
    bx_ntvdm_dpmi_startup_source_initialize_app();
}
VOID DpmiPassTableAddress(
    VOID
    )
/*++

Routine Description:

    This routine stores the flat address for the LDT table in the 16bit
    land (pointed to by selGDT in 16bit land).

Arguments:

    None

Return Value:

    None.

--*/
{

    /* DIVERGENCE (T257 S3): retain only the source's selGDT publication
     * event.  Native Bochs owns descriptor tables; no `Ldt`/`IntelBase` host
     * address or copied descriptor cache is created. */
    bx_ntvdm_dpmi_startup_source_publish_selector_table();

}
