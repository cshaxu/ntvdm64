#ifndef WOW_DIALOG_CREATION_BINDING_H
#define WOW_DIALOG_CREATION_BINDING_H

#include <windows.h>

/* ADAPTER-WOW-063: scoped entry to original InternalCreateDialog. Native
 * HWND creation uses the common explicit enrollment owner; no separate
 * dialog property, hook, parser or callback scheduler is retained. */
HWND WINAPI wow_user_server_create_dialog(HANDLE, LPDLGTEMPLATE, DWORD, HWND,
    DLGPROC, LONG, UINT);
/* Modal creation reaches the same original InternalCreateDialog body through
 * clres.c.  Keep its callback/borrow/context lease identical to the
 * registered modeless entry; clres.c still owns template sizing, flags and
 * the modal loop. */
int WINAPI wow_user_modal_dialog_box(HINSTANCE, LPCDLGTEMPLATEW, HWND,
    DLGPROC, LPARAM, UINT);
/* Original modal/modeless keyboard processing shares one scoped entry. */
BOOL WINAPI wow_dialog_messageA(HWND, LPMSG);
BOOL WINAPI wow_dialog_messageW(HWND, LPMSG);

#endif
