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

struct wow_task_order_thread;
struct wow_window_creation_enrollment;
struct wow_window_words_binding;
/* Shared creation entrance asks the active dialog scope first, before
 * generic class enrollment. Neither operation advances the hook chain. */
BOOL WINAPI wow_dialog_creation_claim(HWND, const CREATESTRUCTA *,
    struct wow_task_order_thread *, struct wow_window_creation_enrollment *, BOOL *);
void WINAPI wow_dialog_creation_cancel(HWND, struct wow_window_words_binding *);

#endif
