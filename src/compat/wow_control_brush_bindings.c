#include <windows.h>
#include <winternl.h>
#include "wow_control_brush_bindings.h"
#include "wow_window_client_state_bindings.h"
#include "wow_user_borrow_scope.h"
#include "wow_user_native_call.h"

static wow_control_brush_provider brush_provider;

BOOL WINAPI wow_control_brush_bind(wow_control_brush_provider provider)
{
    wow_control_brush_provider previous;
    if (!provider) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    previous = (wow_control_brush_provider)InterlockedCompareExchangePointer(
        (PVOID volatile *)&brush_provider, (PVOID)provider, NULL);
    if (previous && previous != provider) { SetLastError(ERROR_ALREADY_INITIALIZED); return FALSE; }
    return TRUE;
}

VOID WINAPI wow_control_brush_retain(HWND window, struct tagWND *state)
{
    if (!state || !wow_user_borrow_client_state(window, state))
        wow_user_native_call_failed("control-brush-retain", ERROR_INVALID_STATE);
}

HBRUSH WINAPI wow_control_brush_color(HWND receiver, HWND source,
    HDC dc, UINT message)
{
    /* Both exact allocations are retained before the potentially reentrant
     * color service. Read handles before entering native USER, never after. */
    return wow_edit_NtUserGetControlColor(receiver, source, dc, message);
}

HBRUSH WINAPI wow_control_brush(HWND window, HDC dc, UINT message)
{
    DWORD error = GetLastError();
    HBRUSH result = NULL;
    struct tagWND *state;
    wow_user_borrow_scope scope;
    if (!wow_user_borrow_enter(&scope)) return NULL;
    __try {
        state = wow_window_client_validate_hwnd(window);
        /* XP NtUserGetControlBrush validates these before its retained body. */
        if (state && dc && message >= WM_CTLCOLORMSGBOX &&
                message <= WM_CTLCOLORSTATIC) {
            wow_control_brush_retain(window, state);
            SetLastError(error);
            if (!brush_provider) RaiseFailFastException(NULL, NULL, 0);
            result = brush_provider(state, dc, message);
        }
    } __finally {
        wow_user_borrow_leave_required(&scope);
    }
    return result;
}
