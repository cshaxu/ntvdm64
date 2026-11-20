#ifndef COMPAT_WOW_NATIVE_DIALOG_H
#define COMPAT_WOW_NATIVE_DIALOG_H
#include "wow_native_controls.h"

/* Installed USER services. Callers own identity conversion and execution scopes. */
BOOL WINAPI wow_dialog_native_ready(void);
BOOL WINAPI wow_dialog_current_foreground(void);
BOOL WINAPI wow_dialog_window_foreground(HWND);
POINT WINAPI wow_dialog_cursor_position(void);
BOOL WINAPI wow_dialog_native_zap_active_and_focus(void);
BOOL WINAPI wow_dialog_native_set_thread_state(DWORD, DWORD);
BOOL WINAPI wow_dialog_native_set_system_menu(HWND);
/* After wow_dialog_native_ready succeeded; caller supplies a resolved atom,
 * full native parent and counted native title. No identity or task lookup. */
HWND WINAPI wow_native_create_dialog_window(DWORD, ATOM,
    const wow_native_text_descriptor *, DWORD, int, int, int, int,
    HWND, HMENU, HANDLE, LPVOID, DWORD);
#endif
