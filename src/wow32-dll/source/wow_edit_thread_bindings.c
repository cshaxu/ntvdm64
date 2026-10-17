#include "wow_edit_call_bindings.h"
#include "wow_user_client_view_layout.h"
#include "wow_user_borrow_scope.h"
#include "wow_user_task_lifecycle.h"

/* ADAPTER-WOW-061/063: preserve original ECGetControlBrush's same-thread
 * decision. No THREADINFO is fabricated, dereferenced or registered here. */
DWORD WINAPI wow_edit_window_thread_id(PWND state)
{
    DWORD error = GetLastError(), thread_id;
    wow_window_words_binding *binding;
    if (!state)
        wow_user_native_call_failed("edit-brush-thread-state", ERROR_INVALID_STATE);
    binding = wow_user_borrow_client_state(state->head.h, state);
    if (!binding)
        wow_user_native_call_failed("edit-brush-thread-owner", GetLastError());
    thread_id = wow_window_words_thread_id(binding);
    if (!thread_id)
        wow_user_native_call_failed("edit-brush-thread-identity", ERROR_INVALID_STATE);
    SetLastError(error);
    return thread_id;
}
