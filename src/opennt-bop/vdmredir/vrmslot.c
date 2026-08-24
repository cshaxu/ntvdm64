/*
 * Directly retained OpenNT VDMREDIR source body.
 *
 * Original source: refs/opennt/base/mvdm/vdmredir/vrmslot.c,
 * VrPeekMailslot.  The full original unit is being recovered in staged
 * owner groups; this first body deliberately supersedes the former
 * source-derived peek emulation because OpenNT explicitly declines peeking
 * NT mailslots.
 */

#include "vrmslot_compat.h" /* DIVERGENCE(BOP-DIV-057): NT4 vdmredir/CCPU
                               * header closure is replaced by the declared
                               * same-shaped adapter-softpc facade. */

VOID
VrPeekMailslot(
    VOID
    )

/*++

Routine Description:

    Performs DosPeekMailslot request on behalf of VDM redir.

    Note: we are not supporting Peeks of NT mailslots (the Win32 Mailslot API
    does not support mailslot peek). This routine is left here as a place
    holder should we want to descend to the NT level to implement mailslots
    (which do allow peeks)

Arguments:

    None. All arguments are extracted from 16-bit context descriptor

Return Value:

    None. Returns values in VDM Ax and Flags registers

--*/

{
    /*
     * return not supported error instead of ERROR_INVALID_FUNCTION
     */

    SET_ERROR(ERROR_NOT_SUPPORTED);
}
