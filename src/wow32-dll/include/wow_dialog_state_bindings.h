#ifndef WOW_DIALOG_STATE_BINDINGS_H
#define WOW_DIALOG_STATE_BINDINGS_H
#include "wow_window_client_state_bindings.h"
#include "wow_user_client_view_layout.h"
#define WOW_ORIGINAL_DIALOG_LAYOUT
#include "opennt-host/windows/core/ntuser/inc/user.h"
#undef WOW_ORIGINAL_DIALOG_LAYOUT

/* Caller retains the existing HWND/task borrow. This only publishes the
 * original caller-owned PDLG; it does not allocate or free dialog resources.
 * Clearing during original final destruction precedes identity withdrawal. */
BOOL WINAPI wow_dialog_set_pointer(HWND, PDLG);
/* Original VOID service boundary: rejected storage cannot be ignored. */
VOID WINAPI wow_dialog_set_pointer_word(HWND, UINT);
/* Canonical original final destruction, under the existing task/borrow
 * scope, before the enclosing HWND binding withdraws its identity. */
BOOL WINAPI wow_dialog_finalize_bound(HWND);
LRESULT CALLBACK wow_dialog_window_procA(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK wow_dialog_window_procW(HWND, UINT, WPARAM, LPARAM);
/* Select only already recovered state. A handled zero/error does not permit
 * a second call into modern USER's unrelated PDLG. */
BOOL WINAPI wow_dialog_default_message(HWND, UINT, WPARAM, LPARAM, LONG *);
#endif
