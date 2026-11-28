/****************************** Module Header ******************************\
* Module Name: hdata.c
*
* Copyright (c) 1985 - 1999, Microsoft Corporation
*
* DDE Manager data handle functions
*
* Created: 11/12/91 Sanford Staab
*
\***************************************************************************/

#define DDEMLDB
#include "wow_dde_data_bindings.h"
#include "w32w64.h"
#pragma hdrstop
#line 476
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
        cb = (DWORD)GlobalSize(hDDE);
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



HBITMAP CopyBitmap(
HBITMAP hbm)
{
    BITMAP bm;
    HBITMAP hbm2 = NULL, hbmOld1, hbmOld2;
    HDC hdc, hdcMem1, hdcMem2;

    if (!GetObject(hbm, sizeof(BITMAP), &bm)) {
        return(0);
    }
    hdc = NtUserGetDC(NULL);  // screen DC
    if (!hdc) {
        return(0);
    }
    hdcMem1 = CreateCompatibleDC(hdc);
    if (!hdcMem1) {
        goto Cleanup3;
    }
    hdcMem2 = CreateCompatibleDC(hdc);
    if (!hdcMem2) {
        goto Cleanup2;
    }
    hbmOld1 = SelectObject(hdcMem1, hbm);
    hbm2 = CreateCompatibleBitmap(hdcMem1, bm.bmWidth, bm.bmHeight);
    if (!hbm2) {
        goto Cleanup1;
    }
    hbmOld2 = SelectObject(hdcMem2, hbm2);
    BitBlt(hdcMem2, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem1, 0, 0, SRCCOPY);
    SelectObject(hdcMem1, hbmOld1);
    SelectObject(hdcMem2, hbmOld2);
Cleanup1:
    DeleteDC(hdcMem2);
Cleanup2:
    DeleteDC(hdcMem1);
Cleanup3:
    NtUserReleaseDC(NULL, hdc);
    return(hbm2);
}


HPALETTE CopyPalette(
HPALETTE hpal)
{
    int cPalEntries;
    LOGPALETTE *plp;

    if (!GetObject(hpal, sizeof(int), &cPalEntries)) {
        return(0);
    }
    plp = (LOGPALETTE *)DDEMLAlloc(sizeof(LOGPALETTE) +
            (cPalEntries - 1) * sizeof(PALETTEENTRY));
    if (!plp) {
        return(0);
    }
    if (!GetPaletteEntries(hpal, 0, cPalEntries, plp->palPalEntry)) {
        DDEMLFree(plp);
        return(0);
    }
    plp->palVersion = 0x300;
    plp->palNumEntries = (WORD)cPalEntries;
    hpal = CreatePalette(plp);
    if (hpal != NULL) {
        if (!SetPaletteEntries(hpal, 0, cPalEntries, plp->palPalEntry)) {
            DeleteObject(hpal);
            hpal = NULL;
        }
    }
    DDEMLFree(plp);
    return(hpal);
}



/***************************************************************************\
* CopyDDEData
*
* Description:
* Used to copy DDE data apropriately.
*
* History:
* 11-19-91 sanfords Created.
\***************************************************************************/
HANDLE CopyDDEData(
HANDLE hDDE,
BOOL fIsExecute)
{
    HANDLE hDDENew;
    PDDE_DATA pdde, pddeNew;
    LPMETAFILEPICT pmfPict;
    HANDLE hmfPict;

    hDDENew = UserGlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE,
            UserGlobalSize(hDDE));
    if (!hDDENew) {
        return (0);
    }
    USERGLOBALLOCK(hDDE, pdde);
    if (pdde == NULL) {
        UserGlobalFree(hDDENew);
        return (0);
    }
    USERGLOBALLOCK(hDDENew, pddeNew);
    UserAssert(pddeNew);
    RtlCopyMemory(pddeNew, pdde, UserGlobalSize(hDDE));

    if (!fIsExecute) {
        switch (pdde->wFmt) {
        case CF_BITMAP:
        case CF_DSPBITMAP:
            pddeNew->Data = (KERNEL_PVOID)CopyBitmap((HBITMAP)pdde->Data);
            break;

        case CF_PALETTE:
            pddeNew->Data = (KERNEL_PVOID)CopyPalette((HPALETTE)pdde->Data);
            break;

        case CF_DIB:
            pddeNew->Data = (KERNEL_PVOID)CopyDDEData((HANDLE)pdde->Data, TRUE);
            break;

        case CF_METAFILEPICT:
        case CF_DSPMETAFILEPICT:
            hmfPict = CopyDDEData((HANDLE)pdde->Data, TRUE);
            USERGLOBALLOCK(hmfPict, pmfPict);
            if (pmfPict == NULL) {
                WOWGLOBALFREE(hmfPict);
                USERGLOBALUNLOCK(hDDENew);
                WOWGLOBALFREE(hDDENew);
                USERGLOBALUNLOCK(hDDE);
                return (FALSE);
            }
            pmfPict->hMF = CopyMetaFile(pmfPict->hMF, NULL);
            USERGLOBALUNLOCK(hmfPict);
            pddeNew->Data = (KERNEL_PVOID)hmfPict;
            break;

        case CF_ENHMETAFILE:
        case CF_DSPENHMETAFILE:
            pddeNew->Data = (KERNEL_PVOID)CopyEnhMetaFile((HANDLE)pdde->Data, NULL);
            break;
        }
    }
    USERGLOBALUNLOCK(hDDENew);
    USERGLOBALUNLOCK(hDDE);
    return (hDDENew);
}
