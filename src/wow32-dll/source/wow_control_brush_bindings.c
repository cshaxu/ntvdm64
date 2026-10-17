#include "wow_control_brush_bindings.h"
#include "wow_window_client_state_bindings.h"
#include "wow_edit_call_bindings.h"
#include "wow_user_task_lifecycle.h"

HBRUSH WINAPI xxxGetControlBrush(PWND, HDC, UINT);

VOID WINAPI wow_control_brush_retain(PWND state)
{
    if (!state || !wow_user_borrow_client_state(state->head.h, state))
        wow_user_native_call_failed("control-brush-retain", ERROR_INVALID_STATE);
}

HBRUSH WINAPI wow_control_brush_color(PWND recipient, PWND control,
    HDC dc, UINT message)
{
    /* Both exact allocations are retained before the potentially reentrant
     * color service. Read handles before entering native USER, never after. */
    HWND receiver = recipient->head.h;
    HWND source = control->head.h;
    return wow_edit_NtUserGetControlColor(receiver, source, dc, message);
}

HBRUSH WINAPI wow_control_brush(HWND window, HDC dc, UINT message)
{
    DWORD error = GetLastError();
    HBRUSH result = NULL;
    PWND state;
    wow_user_borrow_scope scope;
    if (!wow_user_borrow_enter(&scope)) return NULL;
    __try {
        state = wow_window_client_validate_hwnd(window);
        if (state) {
            wow_control_brush_retain(state);
            SetLastError(error);
            result = xxxGetControlBrush(state, dc, message);
        }
    } __finally {
        wow_user_borrow_leave_required(&scope);
    }
    return result;
}
