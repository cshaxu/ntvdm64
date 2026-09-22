/****************************** Module Header ******************************\
* Module Name: hdata.c
*
* DDE Manager data handle functions
*
* Created: 11/12/91 Sanford Staab
*
\***************************************************************************/

/* DIVERGENCE(OPENNT-HOST-032): retain only the original FreeDDEData
 * owner; DDEML instance/transaction/copy APIs require the excluded USER
 * subsystem. Its finite declarations and public API macros are adapter-owned. */
#include "wow_dde_data_bindings.h"

/***************************************************************************\
* FreeDDEData
*
* Description:
* Used for freeing DDE data including any special indirect objects
* associated with the data depending on the format. This function
* SHOULD NOT BE USED TO FREE EXECUTE DATA!
*
* The data is not freed if the fRelease bit is clear and fIgnoreRelease
* is FALSE.
*
*   The fFreeTruelyGlobalObjects parameter is used to distinguish tracking
*   layer frees from DDEML frees.  Data in certain formats (CF_BITMAP,
*   CF_PALETTE) is maintained on the gdi CSR server side.  When this is
*   passed between processes, gdi is not able to maintain multiple process
*   ownership on these objects so the objects must be made global.  Thus
*   the tracking layer should NOT free these objects on behalf of another
*   process because they are truely global- however, DDEML can do this
*   because it is following the protocol which delclares who is in charge
*   of freeing global data.  (YUCK!)
*
* History:
* 11-19-91 sanfords Created.
\***************************************************************************/
/*
 * WARNING: This is exported for NetDDE use - DO NOT CHANGE THE PARAMETERS!
 */
VOID FreeDDEData(
HANDLE hDDE,
BOOL fIgnorefRelease,
BOOL fFreeTruelyGlobalObjects)
{
    PDDE_DATA pdde;
    LPMETAFILEPICT pmfPict;
    DWORD cb;

    USERGLOBALLOCK(hDDE, pdde);
    if (pdde == NULL) {
        return ;
    }

    if ((pdde->wStatus & DDE_FRELEASE) || fIgnorefRelease) {
        cb = GlobalSize(hDDE);
        /*
         * Because there is the possibility that the data never got
         * initialized we need to do this in a try-except so we
         * behave nicely.
         */
        switch (pdde->wFmt) {
        case CF_BITMAP:
        case CF_DSPBITMAP:
        case CF_PALETTE:
            if (cb >= sizeof(HANDLE)) {
                if (fFreeTruelyGlobalObjects) {
                    if (pdde->Data != 0) {
                        DeleteObject((HANDLE)pdde->Data);
                    }
                } else {
                    /*
                     * !fFreeTruelyGlobalObject implies we are only freeing
                     * the Gdi proxy.  (another process may still have this
                     * object in use.)
                     *
                     * ChrisWil: removed this call.  No longer
                     *           applicable in KMode.
                     *
                     * GdiDeleteLocalObject((ULONG)pdde->Data);
                     *
                     */
                }
            }
            break;

        case CF_DIB:
            if (cb >= sizeof(HANDLE)) {
                if (pdde->Data != 0) {
                    WOWGLOBALFREE((HANDLE)pdde->Data);
                }
            }
            break;

        case CF_METAFILEPICT:
        case CF_DSPMETAFILEPICT:
            if (cb >= sizeof(HANDLE)) {
                if (pdde->Data != 0) {
                    USERGLOBALLOCK(pdde->Data, pmfPict);
                    if (pmfPict != NULL) {
                        if (GlobalSize((HANDLE)pdde->Data) >= sizeof(METAFILEPICT)) {
                            DeleteMetaFile(pmfPict->hMF);
                        }
                        USERGLOBALUNLOCK((HANDLE)pdde->Data);
                        WOWGLOBALFREE((HANDLE)pdde->Data);
                    }
                }
            }
            break;

        case CF_ENHMETAFILE:
        case CF_DSPENHMETAFILE:
            if (cb >= sizeof(HANDLE)) {
                if (pdde->Data != 0) {
                    DeleteEnhMetaFile((HANDLE)pdde->Data);
                }
            }
            break;
        }
        USERGLOBALUNLOCK(hDDE);
        WOWGLOBALFREE(hDDE);
    } else {
        USERGLOBALUNLOCK(hDDE);
    }
}

/* DIVERGENCE(OPENNT-HOST-032): remaining original DDEML APIs omitted. */

