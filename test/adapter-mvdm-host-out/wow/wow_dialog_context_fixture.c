#include "wow_dialog_context_bindings.h"
#include "wow_user_task_lifecycle.h"
#include <stdio.h>
#include <string.h>

/* Actual native preference/font acquisition, explicit task/metric doubles.
 * This does not execute original dialog creation or a guest callback. */
static wow_user_runtime_thread current;
static wow_user_runtime runtime;
static wow_user_task_lifecycle lifecycle;
static wow_task_order_thread thread;
static BOOL reject_metrics;
static BOOL reject_geometry;
static int failures;
__declspec(noreturn) void WINAPI wow_user_native_call_failed(const char *site, DWORD error)
{ printf("unexpected failure %s %lu\n", site, error); ExitProcess(99); }
#define CHECK(c) do { if (!(c)) { ++failures; printf("FAIL line=%d\n", __LINE__); } } while (0)
wow_user_runtime_thread *WINAPI wow_user_runtime_current(void) { return &current; }
BOOL WINAPI wow_default_capture_context(wow_default_context *context, UINT message)
{
    CHECK(message == WM_NCHITTEST);
    if (reject_geometry) { SetLastError(ERROR_NOT_READY); return FALSE; }
    ZeroMemory(context, sizeof(*context));
    context->server.gclBorder = 3;
    return TRUE;
}
BOOL WINAPI wow_edit_capture_context(wow_edit_context *context)
{
    if (reject_metrics) { SetLastError(ERROR_NOT_READY); return FALSE; }
    ZeroMemory(context, sizeof(*context));
    context->cxSysFontChar = 8;
    context->cySysFontChar = 16;
    SetRect(&context->screen_rect, 0, 0, 1234, 987);
    return TRUE;
}
int __cdecl main(void)
{
    wow_dialog_context context, before;
    NONCLIENTMETRICSW metrics = {sizeof(metrics)};
    LOGFONTW font;
    RECT work;
    BOOL snap;
    HFONT retained;
    POINT origin = {0};
    HMONITOR monitor;
    MONITORINFO *view, *nested_view, expected = {sizeof(expected)};
    wow_dialog_context nested;
    memset(&context, 0x5a, sizeof(context));
    before = context;
    CHECK(!wow_dialog_capture_context(&context, TRUE));
    CHECK(GetLastError() == ERROR_INVALID_STATE && !memcmp(&context, &before, sizeof(context)));
    current.runtime = &runtime;
    current.thread = &thread;
    runtime.lifecycle = &lifecycle;
    lifecycle.initialized = TRUE;
    thread.ppi = &lifecycle.process;
    lifecycle.cleanup_shared.atomSysClass[0] = 0x1234;
    reject_metrics = TRUE;
    CHECK(!wow_dialog_capture_context(&context, TRUE));
    CHECK(GetLastError() == ERROR_NOT_READY && !memcmp(&context, &before, sizeof(context)));
    reject_metrics = FALSE;
    reject_geometry = TRUE;
    CHECK(!wow_dialog_capture_context(&context, TRUE));
    CHECK(GetLastError() == ERROR_NOT_READY && !memcmp(&context, &before, sizeof(context)));
    reject_geometry = FALSE;
    SetLastError(44763);
    CHECK(wow_dialog_capture_context(&context, TRUE));
    CHECK(GetLastError() == 44763);
    CHECK(context.server.atomSysClass[0] == 0x1234);
    CHECK(context.geometry.server.gclBorder == 3);
    CHECK(context.server.cxSysFontChar == 8 && context.server.cySysFontChar == 16);
    CHECK(context.server.screen_rect.right == 1234 &&
        context.server.screen_rect.bottom == 987);
    CHECK(context.server.dmLogPixels > 0 && context.server.hMsgFont);
    CHECK(SystemParametersInfoW(SPI_GETSNAPTODEFBUTTON, 0, &snap, 0));
    CHECK(!!(context.server.PUSIFlags & 0x02) == !!snap);
    CHECK(SystemParametersInfoW(SPI_GETKEYBOARDPREF, 0, &snap, 0));
    CHECK(context.server.bKeyboardPref == snap);
    CHECK(SystemParametersInfoW(SPI_GETKEYBOARDCUES, 0, &snap, 0));
    CHECK(!!(context.server.PUSIFlags & 0x20) == !!snap);
    monitor = MonitorFromPoint(origin, MONITOR_DEFAULTTOPRIMARY);
    CHECK(monitor && GetMonitorInfoW(monitor, &expected));
    wow_dialog_current = &context;
    SetLastError(44765);
    view = wow_dialog_monitor(monitor);
    CHECK(view && !memcmp(view, &expected, sizeof(expected)) && GetLastError() == 44765);
    CHECK(wow_dialog_monitor(monitor) == view && wow_dialog_primary_monitor() == view);
    CHECK(!wow_dialog_monitor(NULL));
    CHECK(wow_dialog_capture_context(&nested, FALSE));
    wow_dialog_current = &nested;
    nested_view = wow_dialog_monitor(monitor);
    CHECK(nested_view != view && !memcmp(nested_view, view, sizeof(*view)));
    wow_dialog_release_context(&nested);
    CHECK(!nested.monitors);
    wow_dialog_current = &context;
    CHECK(wow_dialog_monitor(monitor) == view);
    CHECK(SystemParametersInfoW(SPI_GETWORKAREA, 0, &work, 0));
    CHECK(EqualRect(&work, &context.server.rcWork));
    CHECK(SystemParametersInfoW(SPI_GETSNAPTODEFBUTTON, 0, &snap, 0));
    CHECK(snap == context.server.fSnapTo);
    CHECK(SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(metrics), &metrics, 0));
    CHECK(GetObjectW(context.server.hMsgFont, sizeof(font), &font) == sizeof(font));
    CHECK(font.lfHeight == metrics.lfMessageFont.lfHeight &&
        font.lfWeight == metrics.lfMessageFont.lfWeight &&
        !lstrcmpW(font.lfFaceName, metrics.lfMessageFont.lfFaceName));
    retained = context.server.hMsgFont;
    SetLastError(44764);
    wow_dialog_release_context(&context);
    CHECK(!context.monitors);
    wow_dialog_current = NULL;
    CHECK(!context.server.hMsgFont && GetLastError() == 44764);
    CHECK(!GetObjectW(retained, sizeof(font), &font));
    wow_dialog_release_context(&context);
    CHECK(wow_dialog_capture_context(&context, FALSE));
    CHECK(!context.server.hMsgFont);
    wow_dialog_release_context(&context);
    CHECK(!wow_dialog_current);
    printf("dialog_context_failures=%d\n", failures);
    return failures != 0;
}
