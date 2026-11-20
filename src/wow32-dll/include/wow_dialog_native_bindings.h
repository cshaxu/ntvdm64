#ifndef WOW_DIALOG_NATIVE_BINDINGS_H
#define WOW_DIALOG_NATIVE_BINDINGS_H
#include "compat/include/wow_native_dialog.h"
#define WOW_ORIGINAL_LARGE_STRING_LAYOUT
#include "wow32-dll/include/opennt_user_multipass.h"
#undef WOW_ORIGINAL_LARGE_STRING_LAYOUT

/* ADAPTER-WOW-062: native operation only, after the existing class owner
 * resolved/retained a native atom. Caller retains all descriptors, HWND/class
 * ownership and task/native-call scope through synchronous callbacks.
 * No original WOW-word array is passed into modern USER private storage.
 * The caller separately captures full module version and creation flags. */
/* Original EndDialog call boundary: restores the existing WOW/native scope
 * even on unwind. A missing service cannot silently become a successful
 * VOID operation after EndDialog has already changed dialog state. */
VOID WINAPI wow_dialog_zap_active_and_focus(void);
VOID WINAPI wow_dialog_set_thread_state(DWORD, DWORD);
BOOL WINAPI wow_dialog_set_system_menu(HWND);
HWND WINAPI wow_dialog_native_create_window(DWORD ex_style, ATOM class_atom,
    const LARGE_STRING *title, DWORD style, int x, int y, int width, int height,
    HWND parent, HMENU menu, HANDLE module, LPVOID parameter, DWORD version);
#endif
