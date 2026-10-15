/****************************** Module Header ******************************\
* Module Name: handles.c
*
* Copyright (c) 1985-92, Microsoft Corporation
*
* HANDLES.C - Data handle manager
*
* History:
* 10-28-91 Sanfords Created
\***************************************************************************/

/* DIVERGENCE(OPENNT-HOST-069): true subset, unchanged GetFullUserHandle.
 * The including host binding supplies the existing handle-table view;
 * unrelated DDEML allocation, callbacks and manager state are not selected. */

DWORD GetFullUserHandle(WORD wHandle)
{
    DWORD dwHandle;
    PHE phe;

    dwHandle = HMIndexFromHandle(wHandle);

    if (dwHandle < gpsi->cHandleEntries) {

        phe = &gSharedInfo.aheList[dwHandle];

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
