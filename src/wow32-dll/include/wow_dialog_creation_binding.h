#ifndef WOW_DIALOG_CREATION_BINDING_H
#define WOW_DIALOG_CREATION_BINDING_H

#include <windows.h>

/* ADAPTER-WOW-063: scoped entry to original InternalCreateDialog. Native
 * HWND creation uses the common explicit enrollment owner; no separate
 * dialog property, hook, parser or callback scheduler is retained. */
HWND WINAPI wow_user_server_create_dialog(HANDLE, LPDLGTEMPLATE, DWORD, HWND,
    DLGPROC, LONG, UINT);
/* Original modal/modeless keyboard processing shares one scoped entry. */
BOOL WINAPI wow_dialog_messageA(HWND, LPMSG);
BOOL WINAPI wow_dialog_messageW(HWND, LPMSG);

#endif
