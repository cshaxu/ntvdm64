/* Copyright (c) 1985-92, Microsoft Corporation.
 * OpenNT windows/core/ntuser/client/handles.c:293-320.
 * Revision 5c5b979ec08c17d3ca2eb70e8aad62d26515d01c.
 * Include in the translation unit providing the shared handle bindings.
 */
#ifndef WOW_FULL_USER_HANDLE_H
#define WOW_FULL_USER_HANDLE_H

DWORD GetFullUserHandle(WORD wHandle)
{
    DWORD dwHandle;
    PHE phe;

    dwHandle = HMIndexFromHandle(wHandle);

    if (dwHandle < gpsi->cHandleEntries) {

        phe = WowSharedHandleEntry(dwHandle);

        if (phe->bType == TYPE_WINDOW)
            return(MAKELONG(dwHandle, phe->wUniq));
    }

    /*
     * object may be gone, but we must pass something.
     * DDE terminates will fail if we don't map this right even after
     * the window is dead!
     *
     * NOTE: This fix will only work for WOW apps, but since the 32bit
     * tracking layer locks dde windows until the last terminate is
     * received, we won't see this problem on the 32bit side.
     *
     * BUG: We WILL see a problem for OLE32 thunked DDE though.
     */
    return(wHandle);
}

#endif
