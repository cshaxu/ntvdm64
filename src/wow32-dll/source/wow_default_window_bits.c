#include "non-mvdm/public/sdk/inc/nt.h"
/* Copyright (c) 1985-1995, Microsoft Corporation.
 * Derived from OpenNT windows/core/ntuser/client/client.c, revision
 * 5c5b979ec08c17d3ca2eb70e8aad62d26515d01c.
 * T483 S6 P24 owner-approved extraction: WowGetDefWindowProcBits.
 * Original function bodies and existing host bindings are preserved.
 */
#include "compat/include/opennt_user_multipass.h"
#include "wow_default_message_bindings.h"
#define gSharedInfo wow_default_message_state
#define UserAssert(exp) ((void)sizeof(exp))
/**************************************************************************\
* WowGetDefWindowProcBits - Fills in bit array for WOW
*
* 22-Jul-1991 mikeke    Created
\**************************************************************************/

WORD WowGetDefWindowProcBits(
    PBYTE    pDefWindowProcBits,
    WORD     cbDefWindowProcBits)
{
    WORD  wMaxMsg;
    PBYTE pbSrc, pbDst, pbDstEnd;

    /*
     * Merge the bits from gpsi->gabDefWindowMsgs and
     * gpsi->gabDefWindowSpecMsgs into WOW's DefWindowProcBits.  These two
     * indicate which messages must go directly to the server and which
     * can be handled with some special code in DefWindowProcWorker.
     * Bitwise OR'ing the two gives a bit array with 1 in the bit field
     * for each message that must go to user32's DefWindowProc, and 0
     * for those that can be returned back to the client immediately.
     *
     * For speed we assume WOW has zeroed the buffer, in fact it's in
     * USER.EXE's code segment and is zeroed in the image.
     */

    wMaxMsg = max(gSharedInfo.DefWindowMsgs.maxMsgs,
            gSharedInfo.DefWindowSpecMsgs.maxMsgs);

    UserAssert((wMaxMsg / 8 + 1) <= cbDefWindowProcBits);

    //
    // If the above assertion fires, the size of the DWPBits array in
    // \nt\private\mvdm\wow16\user\user.asm needs to be increased.
    //

    /* First copy the bits from DefWindowMsgs */

    RtlCopyMemory(
        pDefWindowProcBits,
        gSharedInfo.DefWindowMsgs.abMsgs,
        gSharedInfo.DefWindowMsgs.maxMsgs / 8 + 1
        );

    /* Next OR in the bits from DefWindowSpecMsgs */

    pbSrc = gSharedInfo.DefWindowSpecMsgs.abMsgs;
    pbDst = pDefWindowProcBits;
    pbDstEnd = pbDst + (gSharedInfo.DefWindowSpecMsgs.maxMsgs / 8 + 1);

    while (pbDst < pbDstEnd)
    {
        *pbDst++ |= *pbSrc++;
    }

    return wMaxMsg;
}

/* Preserve the original merge's zero-input precondition without exporting
 * its private table lengths to the caller. */
WORD WINAPI wow_default_message_copy(PBYTE bits, WORD count)
{
    UINT maximum = max(gSharedInfo.DefWindowMsgs.maxMsgs,
        gSharedInfo.DefWindowSpecMsgs.maxMsgs);
    if (!bits || maximum > MAXWORD || count <= maximum / 8u) return 0;
    ZeroMemory(bits, maximum / 8u + 1u);
    return WowGetDefWindowProcBits(bits, count);
}
