#include <windows.h>
#include <winternl.h>
/* Copyright (c) 1985-1995, Microsoft Corporation.
 * Derived from OpenNT windows/core/ntuser/client/client.c, revision
 * 5c5b979ec08c17d3ca2eb70e8aad62d26515d01c.
 * T483 S6 P24 owner-approved extraction: WOWDlgInit, GetEditDS, ReleaseEditDS.
 * Original function bodies and existing host bindings are preserved.
 */
#include "wow_edit_memory_bindings.h"

/***************************************************************************\
* WOWDlgInit
*
* This is a callback to WOW used at the begining of dialog creation to allow
* it to associate the lParam of the WM_INITDIALOG messasge with the window
* prior to actually recieving the message.
*
* 06-19-92 sanfords Created
\***************************************************************************/
DWORD WOWDlgInit(
HWND hwndDlg,
LONG lParam)
{
    UserAssert(pfnInitDlgCallback != NULL);

    return (*pfnInitDlgCallback)(hwndDlg, lParam);
}



/***************************************************************************\
* GetEditDS
*
* This is a callback to WOW used to allocate a segment for DS_LOCALEDIT
* edit controls.  The segment is disguised to look like a WOW hInstance.
*
* 06-19-92 sanfords Created
\***************************************************************************/
HANDLE GetEditDS()
{
    UserAssert(pfn16GlobalAlloc != NULL);

    return((HANDLE)((*pfn16GlobalAlloc)(GHND | GMEM_SHARE, 256)));
}



/***************************************************************************\
* ReleaseEditDS
*
* This is a callback to WOW used to free a segment for DS_LOCALEDIT
* edit controls.
*
* 06-19-92 sanfords Created
\***************************************************************************/
VOID ReleaseEditDS(
HANDLE h)
{
    UserAssert(pfn16GlobalFree != NULL);

    (*pfn16GlobalFree)(LOWORD(h));
}
