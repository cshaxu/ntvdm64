#include <windows.h>
#include <winternl.h>
#include "wow_dialog_context_bindings.h"
#include "wow_user_task_lifecycle.h"
#include "wow_user_failure.h"
#include "wow_standard_class_bindings.h"

typedef struct wow_dialog_monitor_view {
    struct wow_dialog_monitor_view *next;
    HMONITOR handle;
    MONITORINFO info;
} wow_dialog_monitor_view;

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
    BOOL keyboard_cues;
    if (!context) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    if (!runtime || !runtime->thread || !lifecycle || !lifecycle->initialized ||
            runtime->thread->ppi != &lifecycle->process) {
        SetLastError(ERROR_INVALID_STATE); return FALSE;
    }
    if (!wow_edit_capture_context(&value.server)) return FALSE;
    if (!wow_standard_classes_copy_client_atoms(&lifecycle->cleanup_shared,
            value.server.atomSysClass, ARRAYSIZE(value.server.atomSysClass))) return FALSE;
    if (!SystemParametersInfoW(SPI_GETSNAPTODEFBUTTON, 0, &value.server.fSnapTo, 0) ||
            !SystemParametersInfoW(SPI_GETWORKAREA, 0, &value.server.rcWork, 0)) return FALSE;
    if (value.server.fSnapTo) value.server.PUSIFlags |= 0x00000002u; /* XP PUSIF_SNAPTO. */
    if (!SystemParametersInfoW(SPI_GETKEYBOARDPREF, 0, &value.server.bKeyboardPref, 0) ||
            !SystemParametersInfoW(SPI_GETKEYBOARDCUES, 0, &keyboard_cues, 0)) return FALSE;
    /* XP user.h PUSIF_KEYBOARDCUES. Other UI flags come from the shared capture. */
    if (keyboard_cues) value.server.PUSIFlags |= 0x00000020u;
    dc = GetDC(NULL);
    if (!dc) return FALSE;
    value.server.dmLogPixels = GetDeviceCaps(dc, LOGPIXELSY);
    ReleaseDC(NULL, dc);
    if (value.server.dmLogPixels <= 0) {
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
    while (context && context->monitors) {
        wow_dialog_monitor_view *view = context->monitors;
        context->monitors = view->next;
        HeapFree(GetProcessHeap(), 0, view);
    }
    if (context && context->server.hMsgFont) {
        DeleteObject(context->server.hMsgFont);
        context->server.hMsgFont = NULL;
    }
    SetLastError(error);
}

MONITORINFO *WINAPI wow_dialog_monitor(HMONITOR monitor)
{
    DWORD error = GetLastError();
    wow_dialog_monitor_view *view;
    MONITORINFO info = {sizeof(info)};
    if (!monitor) return NULL; /* MONITOR_DEFAULTTONULL is an original decision. */
    if (!wow_dialog_current)
        wow_user_native_call_failed("dialog-monitor-context", ERROR_INVALID_STATE);
    for (view = wow_dialog_current->monitors; view; view = view->next)
        if (view->handle == monitor) return &view->info;
    if (!GetMonitorInfoW(monitor, &info))
        wow_user_native_call_failed("dialog-monitor", GetLastError());
    view = HeapAlloc(GetProcessHeap(), 0, sizeof(*view));
    if (!view) wow_user_native_call_failed("dialog-monitor-view", ERROR_NOT_ENOUGH_MEMORY);
    view->handle = monitor;
    view->info = info;
    view->next = wow_dialog_current->monitors;
    wow_dialog_current->monitors = view;
    SetLastError(error);
    return &view->info;
}

MONITORINFO *WINAPI wow_dialog_primary_monitor(void)
{
    POINT origin = {0};
    return wow_dialog_monitor(MonitorFromPoint(origin, MONITOR_DEFAULTTOPRIMARY));
}
