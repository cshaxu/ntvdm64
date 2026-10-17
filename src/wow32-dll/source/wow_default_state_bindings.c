#include "wow_default_window_bindings.h"

/* ADAPTER-WOW-063: inputs to original compatibility/color decisions, not a
 * replacement for them. Capture before the original body/name-reader lock.
 * Only the branches which read display state need to acquire that state. */
BOOL WINAPI wow_default_capture_context(wow_default_context *context, UINT message)
{
    DWORD error = GetLastError();
    unsigned i;
    if (!context) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    if (!wow_default_messages_ready()) return FALSE;
    ZeroMemory(context, sizeof(*context));
    context->server.uiShellMsg = wow_default_message_state.uiShellMsg;
    /* rare.c SPI_GETBORDER returns gpsi->gclBorder directly. Do not derive
     * logical border units from modern padded frame pixel metrics. */
    if (message == WM_NCHITTEST &&
            !SystemParametersInfoW(SPI_GETBORDER, 0,
                &context->server.gclBorder, 0)) return FALSE;
    if (message == WM_CTLCOLOR ||
            (message >= WM_CTLCOLORMSGBOX && message <= WM_CTLCOLORSTATIC)) {
        if (!wow_edit_drawing_ready()) return FALSE;
        for (i = 0; i < WOW_EDIT_COLOR_COUNT; ++i)
            context->server.argbSystem[i] = GetSysColor(i);
        if (message == WM_CTLCOLORSCROLLBAR) {
            HDC dc = GetDC(NULL);
            int planes, bits;
            if (!dc) return FALSE;
            planes = GetDeviceCaps(dc, PLANES);
            bits = GetDeviceCaps(dc, BITSPIXEL);
            ReleaseDC(NULL, dc);
            if (planes <= 0 || bits <= 0) {
                SetLastError(ERROR_INVALID_DATA); return FALSE;
            }
            context->screen.BitCount = (UINT)planes * (UINT)bits;
        }
    }
    SetLastError(error);
    return TRUE;
}

BOOL WINAPI wow_default_ensure_shell_message(HWND window)
{
    DWORD error = GetLastError();
    /* Only clmsg.c's NULL initialization call is admitted. Never forward
     * to the host's desktop-global task-manager registration API. */
    if (window || !wow_default_current) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    if (!wow_default_messages_ready()) return FALSE;
    wow_default_current->server.uiShellMsg = wow_default_message_state.uiShellMsg;
    SetLastError(error);
    return TRUE;
}
