#ifndef WOW_DIALOG_NATIVE_BINDINGS_H
#define WOW_DIALOG_NATIVE_BINDINGS_H
#include <windows.h>
#define WOW_ORIGINAL_LARGE_STRING_LAYOUT
#include "opennt-host/windows/core/ntuser/inc/user.h"
#undef WOW_ORIGINAL_LARGE_STRING_LAYOUT

/* ADAPTER-WOW-062: native operation only, after the existing class owner
 * resolved/retained a native atom. Caller retains all descriptors, HWND/class
 * ownership and task/native-call scope through synchronous callbacks.
 * No original WOW-word array is passed into modern USER private storage.
 * The caller separately captures full module version and creation flags. */
BOOL WINAPI wow_dialog_native_ready(void);
/* Exact native modal-return queue predicates, not foreground thread-ID
 * comparisons. Missing binding raises the existing adaptation failure. */
BOOL WINAPI wow_dialog_current_foreground(void);
BOOL WINAPI wow_dialog_window_foreground(HWND);
POINT WINAPI wow_dialog_cursor_position(void);
/* Original operation returns VOID. FALSE here means dependency resolution
 * failed; TRUE is not a status reported by the native service. */
BOOL WINAPI wow_dialog_native_zap_active_and_focus(void);
/* Original EndDialog call boundary: restores the existing WOW/native scope
 * even on unwind. A missing service cannot silently become a successful
 * VOID operation after EndDialog has already changed dialog state. */
VOID WINAPI wow_dialog_zap_active_and_focus(void);
BOOL WINAPI wow_dialog_native_set_thread_state(DWORD, DWORD);
VOID WINAPI wow_dialog_set_thread_state(DWORD, DWORD);
BOOL WINAPI wow_dialog_native_set_system_menu(HWND);
BOOL WINAPI wow_dialog_set_system_menu(HWND);
HWND WINAPI wow_dialog_native_create_window(DWORD ex_style, ATOM class_atom,
    const LARGE_STRING *title, DWORD style, int x, int y, int width, int height,
    HWND parent, HMENU menu, HANDLE module, LPVOID parameter, DWORD version);
#endif
