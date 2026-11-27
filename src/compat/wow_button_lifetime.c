#include "wow_button_lifetime.h"
#include "wow_window_client_state_bindings.h"
#include "wow_user_failure.h"
#include "wow_edit_entry_bindings.h"

/* OPENNT-HOST-117: logical cleanup stays original; reuse waits until the
 * oldest same-control invocation returns. No allocation, HWND registry or
 * replacement lookaside pool. The enclosing borrow retains this exact WND. */
static __declspec(thread) wow_button_scope *current_invocation;
static wow_button_data_provider button_data;
static wow_button_release_provider button_release;

BOOL WINAPI wow_button_bind(wow_button_data_provider data, wow_button_release_provider release)
{
    if (!data || !release || (button_data && (button_data != data || button_release != release))) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    button_data = data;
    button_release = release;
    return TRUE;
}

VOID WINAPI wow_button_scope_enter(wow_button_scope *frame, struct tagWND *state)
{
    DWORD error = GetLastError();
    if (!button_data || !button_release)
        wow_user_native_call_failed("button-provider", ERROR_INVALID_STATE);
    if (!frame || !state)
        wow_user_native_call_failed("button-state-scope", ERROR_INVALID_PARAMETER);
    ZeroMemory(frame, sizeof(*frame));
    frame->window = state;
    frame->previous = current_invocation;
    current_invocation = frame;
    SetLastError(error);
}

VOID WINAPI wow_button_scope_leave(wow_button_scope *frame)
{
    DWORD error = GetLastError();
    if (!frame || current_invocation != frame)
        wow_user_native_call_failed("button-call-stack", ERROR_INVALID_STATE);
    current_invocation = frame->previous;
    wow_button_resources_release(&frame->resources);
    if (frame->release_value)
        button_release(frame->release_pool, frame->release_value);
    SetLastError(error);
}

wow_button_resources *WINAPI wow_button_current_resources(void)
{
    wow_button_scope *frame = current_invocation;
    DWORD error = GetLastError();
    if (!frame || !wow_edit_current || !wow_edit_current->screen.BitCount)
        wow_user_native_call_failed("button-drawing-scope", ERROR_INVALID_STATE);
    if (!frame->resources_initialized) {
        if (!wow_button_resources_initialize(&frame->resources))
            wow_user_native_call_failed("button-resource-metadata", GetLastError());
        frame->resources.BitCount = wow_edit_current->screen.BitCount;
        frame->resources_initialized = TRUE;
    }
    SetLastError(error);
    return &frame->resources;
}

wow_edit_context *WINAPI wow_button_current_context(void)
{
    wow_button_resources *resources = wow_button_current_resources();
    wow_edit_current->oembmi = resources->bm;
    return wow_edit_current;
}

BOOL WINAPI wow_button_atlas_blit(HDC dc, int x, int y, int width, int height,
    int source_x, int source_y, DWORD operation)
{
    return wow_button_resources_blit(wow_button_current_resources(), dc,
        x, y, width, height, source_x, source_y, operation);
}

VOID WINAPI wow_button_free_state(PLOOKASIDE pool, PVOID value)
{
    wow_button_scope *frame, *oldest = NULL;
    if (!button_data || !button_release)
        wow_user_native_call_failed("button-provider", ERROR_INVALID_STATE);
    for (frame = current_invocation; frame; frame = frame->previous)
        if (button_data(frame->window) == value) oldest = frame;
    if (!oldest) {
        button_release(pool, value);
        return;
    }
    if (oldest->release_value)
        wow_user_native_call_failed("button-duplicate-state-release", ERROR_INVALID_STATE);
    oldest->release_pool = pool;
    oldest->release_value = value;
}

LRESULT WINAPI wow_button_invoke(HWND window, UINT message, WPARAM wp,
    LPARAM lp, WNDPROC procedure)
{
    wow_button_scope frame;
    DWORD error = GetLastError();
    struct tagWND *state = wow_window_client_validate_hwnd(window);
    LRESULT result;
    if (!state) return 0;
    if (!procedure) { SetLastError(ERROR_INVALID_PARAMETER); return 0; }
    wow_button_scope_enter(&frame, state);
    __try {
        SetLastError(error);
        result = procedure(window, message, wp, lp);
    } __finally {
        wow_button_scope_leave(&frame);
    }
    return result;
}
