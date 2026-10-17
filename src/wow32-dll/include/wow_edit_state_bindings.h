#ifndef WOW_EDIT_STATE_BINDINGS_H
#define WOW_EDIT_STATE_BINDINGS_H
#include "wow_edit_lookaside_bindings.h"
#include "wow_edit_memory_bindings.h"
#include "wow_window_client_state_bindings.h"
#include "opennt-host/windows/core/ntuser/client/usercli.h"

/* Caller retains task/borrow scope and HWND identity through the call.
 * After application destruction dispatch, before retirement; canonical
 * original EDIT/ComboBox/ListBox cleanup, never the application subclass. */
BOOL WINAPI wow_control_finalize_window(HWND);
BOOL WINAPI wow_edit_set_window_fnid(HWND, WORD);

#ifdef WOW_ORIGINAL_EDIT_STATE
#define NtUserSetWindowFNID wow_edit_set_window_fnid
#define NtUserSetWindowLong wow_window_client_set_window_long
#define BEGINRECV_HWND(type, failure, handle) \
    type retval = (failure); \
    PWND pwnd = wow_window_client_validate_hwnd(handle); \
    if (!pwnd) return (failure)
#define ENDRECV_HWND() return retval
#define TRACE(text) ((void)0)
#endif
#endif
