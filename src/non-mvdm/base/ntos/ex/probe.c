/*++

Copyright (c) 1990-1993  Microsoft Corporation

Module Name:

    probe.c

Abstract:

    This module implements the probe for write function.

Author:

    David N. Cutler (davec) 19-Jan-1990

Environment:

    Any mode.

Revision History:

--*/

/* DIVERGENCE(OPENNT-HOST-061): unchanged ProbeForRead; the finite native
 * environment supplies the process address ceiling and status raising. */
#define WOW_ORIGINAL_USER_PROBE
#include "wow_user_probe_bindings.h"

#undef ProbeForRead
NTKERNELAPI
VOID
NTAPI
ProbeForRead(
    IN CONST VOID *Address,
    IN ULONG Length,
    IN ULONG Alignment
    )

/*++

Routine Description:

    This function probes a structure for read accessibility and ensures
    correct alignment of the structure. If the structure is not accessible
    or has incorrect alignment, then an exception is raised.

Arguments:

    Address - Supplies a pointer to the structure to be probed.

    Length - Supplies the length of the structure.

    Alignment - Supplies the required alignment of the structure expressed
        as the number of bytes in the primitive datatype (e.g., 1 for char,
        2 for short, 4 for long, and 8 for quad).

Return Value:

    None.

--*/
{
    PAGED_CODE();

    ASSERT(((Alignment) == 1) || ((Alignment) == 2) ||
           ((Alignment) == 4) || ((Alignment) == 8) ||
           ((Alignment) == 16));

    if ((Length) != 0) {
        if (((ULONG_PTR)(Address) & ((Alignment) - 1)) != 0) {
            ExRaiseDatatypeMisalignment();

        } else if ((((ULONG_PTR)(Address) + (Length)) < (ULONG_PTR)(Address)) ||
                   (((ULONG_PTR)(Address) + (Length)) > (ULONG_PTR)MM_USER_PROBE_ADDRESS)) {
            ExRaiseAccessViolation();
        }
    }
}
