/****************************** Module Header ******************************\
* Module Name: handles.c
*
* Copyright (c) 1985-92, Microsoft Corporation
*
* HANDLES.C - Data handle manager
*
*
* This module allows a 32 bit value to be converted into a handle that
* can be validated with a high probability of correctness.
*
* A handle array is kept which contains the 32 bit data associated with
* it, and a copy of the correect handle value. The handle itself is
* composed of a combination of the index into the array for the associated
* data, the instance value, a type value and a DDEML instance value.
*
* The HIWORD of a handle is guarenteed not to be 0.
*
* History:
* 10-28-91 Sanfords Created
\***************************************************************************/

#line 24
#pragma hdrstop

#line 38
#ifdef TESTING
#line 39
#error Excluded unsupported original TESTING handle-table verification
#line 59
#else
#define CheckHandleTable()
#line 61
#endif // TESTING
#line 293
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
