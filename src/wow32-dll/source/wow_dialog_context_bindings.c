#include "wow_dialog_context_bindings.h"
#include "wow_user_task_lifecycle.h"

__declspec(thread) wow_dialog_context *wow_dialog_current;

BOOL WINAPI wow_dialog_capture_context(wow_dialog_context *context, BOOL font)
{
    DWORD error = GetLastError();
    wow_dialog_context value = {0};
    wow_user_runtime_thread *runtime = wow_user_runtime_current();
    wow_user_task_lifecycle *lifecycle = runtime && runtime->runtime ?
        runtime->runtime->lifecycle : NULL;
    NONCLIENTMETRICSW metrics = {sizeof(metrics)};
    HDC dc;
    if (!context) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    if (!runtime || !runtime->thread || !lifecycle || !lifecycle->initialized ||
            runtime->thread->ppi != &lifecycle->process) {
        SetLastError(ERROR_INVALID_STATE); return FALSE;
    }
    if (!wow_edit_capture_context(&value.server)) return FALSE;
    C_ASSERT(sizeof(value.server.atomSysClass) == sizeof(lifecycle->cleanup_shared.atomSysClass));
    CopyMemory(value.server.atomSysClass, lifecycle->cleanup_shared.atomSysClass,
        sizeof(value.server.atomSysClass));
    if (!SystemParametersInfoW(SPI_GETSNAPTODEFBUTTON, 0, &value.server.fSnapTo, 0) ||
            !SystemParametersInfoW(SPI_GETWORKAREA, 0, &value.server.rcWork, 0)) return FALSE;
    dc = GetDC(NULL);
    if (!dc) return FALSE;
    value.screen.cyPixelsPerInch = GetDeviceCaps(dc, LOGPIXELSY);
    ReleaseDC(NULL, dc);
    if (value.screen.cyPixelsPerInch <= 0) {
        SetLastError(ERROR_INVALID_DATA); return FALSE;
    }
    if (font) {
        if (!wow_default_capture_context(&value.geometry, WM_NCHITTEST)) return FALSE;
        if (!SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(metrics), &metrics, 0))
            return FALSE;
        value.server.hMsgFont = CreateFontIndirectW(&metrics.lfMessageFont);
        if (!value.server.hMsgFont) return FALSE;
    }
    *context = value;
    SetLastError(error);
    return TRUE;
}

VOID WINAPI wow_dialog_release_context(wow_dialog_context *context)
{
    DWORD error = GetLastError();
    if (context && context->server.hMsgFont) {
        DeleteObject(context->server.hMsgFont);
        context->server.hMsgFont = NULL;
    }
    SetLastError(error);
}
