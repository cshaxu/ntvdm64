#define WOW_ORIGINAL_EDIT_CLIENT
#define WOW_ORIGINAL_DIALOG_CLIENT
#define WOW_ORIGINAL_DIALOG_MANAGER
#define WOW_ORIGINAL_DIALOG_END
#include "control-client/precomp.h"
#include <stdio.h>
/* Fixture-owned native setup/observation, outside the emulated call scope. */
#undef PeekMessageW
#undef DestroyWindow

/* Link the actual production-selected dlgend body. WND validation and native
 * mutation services are explicit doubles; real hidden HWNDs supply IsChild
 * and the posted wake message. This is not guest/modal-loop acceptance. */
static DIALOG dialog;
static DLG data;
static WND owner;
static HWND child, active;
static BOOL valid, is_dialog;
static unsigned failures, enables, focuses, hides, activates, zaps;
enum callout { NO_FAULT, VALIDATE, ENABLE, FOCUS, HIDE, ACTIVATE };
static enum callout fault;
static unsigned retirement;
static void callback(enum callout point)
{
    if (point != fault) return;
    /* Exact terminal-state inputs from the existing owner: backing WND
     * remains borrowed, but original dialog cleanup has released PDLG.
     * The real HWND intentionally remains valid to detect numeric-only
     * revalidation or an unwanted wake to a replacement identity. This is
     * a deterministic lifetime double, not Windows handle-reuse evidence. */
    if (retirement & 1) SetWF(&dialog.wnd, WFDESTROYED);
    if (retirement & 2) dialog.wnd.fnid |= FNID_CLEANEDUP_BIT;
    dialog.pdlg = NULL;
}
#define CHECK(x) do { if (!(x)) { ++failures; printf("FAIL line=%d\n", __LINE__); } } while (0)
PWND WINAPI wow_window_client_validate_window(HWND window)
{ return valid && window == dialog.wnd.head.h ? &dialog.wnd : NULL; }
BOOL ValidateDialogPwnd(PWND window)
{ CHECK(window == &dialog.wnd); callback(VALIDATE); return is_dialog; }
HWND WINAPI wow_native_GetActiveWindow(void) { return active; }
HWND WINAPI wow_native_GetFocus(void) { return child; }
BOOL WINAPI wow_native_EnableWindow(HWND window, BOOL enable)
{ CHECK(window == owner.head.h && enable && !data.fEnd && dialog.pdlg); ++enables; callback(ENABLE); return TRUE; }
HWND WINAPI wow_native_SetFocus(HWND window)
{ CHECK(window == dialog.wnd.head.h && data.fEnd && dialog.pdlg); ++focuses; callback(FOCUS); return child; }
BOOL WINAPI wow_native_SetWindowPos(HWND window, HWND after, int x, int y,
    int cx, int cy, UINT flags)
{
    CHECK(window == dialog.wnd.head.h && data.fEnd && dialog.pdlg && !after && !x && !y && !cx && !cy);
    CHECK(flags == (SWP_HIDEWINDOW | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER));
    ++hides; callback(HIDE); return TRUE;
}
HWND WINAPI wow_native_SetActiveWindow(HWND window)
{ CHECK(window == owner.head.h && hides == 1 && dialog.pdlg); ++activates; active = window; callback(ACTIVATE); return dialog.wnd.head.h; }
VOID WINAPI wow_dialog_zap_active_and_focus(void)
{ CHECK(hides == 1); ++zaps; active = NULL; }
static void reset(BOOL owned, BOOL was_active, BOOL disabled)
{
    MSG message;
    while (PeekMessageW(&message, dialog.wnd.head.h, WM_NULL, WM_NULL, PM_REMOVE)) {}
    ZeroMemory(&data, sizeof(data));
    ClrWF(&dialog.wnd, WFDESTROYED);
    dialog.wnd.fnid = FNID_DIALOG;
    fault = NO_FAULT; retirement = 0;
    data.fDisabled = disabled;
    dialog.pdlg = &data;
    dialog.wnd.spwndOwner = owned ? &owner : NULL;
    active = was_active ? dialog.wnd.head.h : NULL;
    valid = is_dialog = TRUE;
    enables = focuses = hides = activates = zaps = 0;
}
int __cdecl main(void)
{
    MSG message;
    unsigned owned, was_active, disabled;
    unsigned point, mode;
    owner.head.h = CreateWindowExW(0, L"STATIC", L"owner", WS_POPUP,
        0, 0, 1, 1, NULL, NULL, GetModuleHandleW(NULL), NULL);
    dialog.wnd.head.h = CreateWindowExW(0, L"STATIC", L"dialog", WS_POPUP,
        0, 0, 1, 1, owner.head.h, NULL, GetModuleHandleW(NULL), NULL);
    child = CreateWindowExW(0, L"STATIC", L"child", WS_CHILD,
        0, 0, 1, 1, dialog.wnd.head.h, NULL, GetModuleHandleW(NULL), NULL);
    if (!owner.head.h || !dialog.wnd.head.h || !child) {
        printf("fixture HWND setup failed error=%lu\n", GetLastError()); return 2;
    }
    for (owned = 0; owned != 2; ++owned)
        for (was_active = 0; was_active != 2; ++was_active)
            for (disabled = 0; disabled != 2; ++disabled) {
                reset(owned, was_active, disabled);
                CHECK(wow_original_EndDialog(dialog.wnd.head.h, -49));
                CHECK(data.fEnd && data.result == -49);
                CHECK(enables == (unsigned)(owned && !disabled));
                CHECK(focuses == was_active && hides == 1);
                CHECK(activates == (unsigned)(owned && was_active));
                CHECK(zaps == (unsigned)(!owned && was_active));
                CHECK(PeekMessageW(&message, dialog.wnd.head.h, WM_NULL, WM_NULL, PM_REMOVE));
                CHECK(!PeekMessageW(&message, dialog.wnd.head.h, WM_NULL, WM_NULL, PM_REMOVE));
            }
    reset(TRUE, TRUE, FALSE); valid = FALSE;
    CHECK(!wow_original_EndDialog(dialog.wnd.head.h, 7));
    CHECK(!data.fEnd && !enables && !focuses && !hides && !activates && !zaps);
    valid = TRUE; is_dialog = FALSE;
    CHECK(!wow_original_EndDialog(dialog.wnd.head.h, 7));
    CHECK(!data.fEnd && !enables && !focuses && !hides && !activates && !zaps);
    CHECK(!PeekMessageW(&message, dialog.wnd.head.h, WM_NULL, WM_NULL, PM_REMOVE));
    for (point = VALIDATE; point <= ACTIVATE; ++point)
        for (mode = 1; mode <= 3; ++mode) {
            BOOL result = FALSE, raised = FALSE;
            reset(TRUE, TRUE, FALSE);
            fault = (enum callout)point; retirement = mode;
            __try { result = wow_original_EndDialog(dialog.wnd.head.h, -49); }
            __except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ?
                    EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { raised = TRUE; }
            CHECK(!raised && !dialog.pdlg && IsWindow(dialog.wnd.head.h));
            CHECK(result == (point >= FOCUS));
            CHECK(data.fEnd == (point >= FOCUS));
            CHECK(data.result == (point >= FOCUS ? -49 : 0));
            CHECK(enables == (unsigned)(point >= ENABLE));
            CHECK(focuses == (unsigned)(point >= FOCUS));
            CHECK(hides == (unsigned)(point >= HIDE));
            CHECK(activates == (unsigned)(point >= ACTIVATE) && !zaps);
            CHECK(!PeekMessageW(&message, dialog.wnd.head.h, WM_NULL, WM_NULL, PM_REMOVE));
        }
    CHECK(DestroyWindow(dialog.wnd.head.h));
    CHECK(!IsWindow(child));
    CHECK(DestroyWindow(owner.head.h));
    printf("dialog-end-original cases=25 failures=%u\n", failures);
    return failures ? 1 : 0;
}
