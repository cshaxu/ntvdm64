#ifndef WOW_EDIT_STATE_BINDINGS_H
#define WOW_EDIT_STATE_BINDINGS_H
#include "wow_edit_lookaside_bindings.h"
#include "wow_edit_memory_bindings.h"
#include "wow_window_client_state_bindings.h"
#include "compat/include/opennt_usercli_multipass.h"

/* Caller retains task/borrow scope and HWND identity through the call.
 * After application destruction dispatch, before retirement; canonical
 * original EDIT/ComboBox/ListBox cleanup, never the application subclass. */
BOOL WINAPI wow_control_finalize_window(HWND);
BOOL WINAPI wow_edit_set_window_fnid(HWND, WORD);

#endif
