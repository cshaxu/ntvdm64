/*++

Copyright (c) 1991  Microsoft Corporation

Module Name:

    vrdll.c

Abstract:

    Initialization for VdmRedir as DLL

    Contents:
        VrDllInitialize

Author:

    Richard L Firth (rfirth) 11-May-1992


Revision History:


--*/

#if DBG
/* DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register. */
#endif

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <vrnmpipe.h>
#include <vrinit.h>
#include "vrdebug.h"

//
// external data
//

//
// external functions
//

extern
VOID
TerminateDlcEmulation(
    VOID
    );


#if DBG
/* DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register. */
#endif

BOOLEAN
VrDllInitialize(
    IN  PVOID DllHandle,
    IN  ULONG Reason,
    IN  PCONTEXT Context OPTIONAL
    )

/*++

Routine Description:

    Gets control as a process or thread attaches/detaches from VdmRedir DLL.
    Always returns success

Arguments:

    DllHandle   -
    Reason      -
    Context     -

Return Value:

    BOOLEAN
        TRUE

--*/

{
    BOOL ok;

#if DBG
/* DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register. */
#endif

    if (Reason == DLL_PROCESS_ATTACH) {

        //
        // we now perform initialization at load time due to deferred loading
        // of VdmRedir.DLL
        //

        ok = VrInitialize();
    } else if (Reason == DLL_PROCESS_DETACH) {

        //
        // clean up resources
        //

        VrUninitialize();
        TerminateDlcEmulation();
        ok = TRUE;
    }

    //
    // basically, nothing to do
    //

    return ok;
}
