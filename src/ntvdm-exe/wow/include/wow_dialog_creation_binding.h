#ifndef WOW_DIALOG_CREATION_BINDING_H
#define WOW_DIALOG_CREATION_BINDING_H

#include <windows.h>

/* ADAPTER-WOW-052: public-USER carrier for the one original
 * InternalCreateDialog responsibility that has no modern public equivalent:
 * publish a WOW WW, invoke the unchanged W32InitDlg after WM_CREATE, and
 * retain it through the dialog's terminal message.  It is deliberately not a
 * replacement dialog-template parser or USER server. */
HWND WINAPI wow_user_server_create_dialog(HANDLE, LPDLGTEMPLATE, DWORD, HWND,
    DLGPROC, LONG, UINT);

#endif
