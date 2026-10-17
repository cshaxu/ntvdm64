#include "wow_edit_entry_bindings.h"
#include "wow_user_client_view_layout.h"
#include "wow_edit_native_bindings.h"
#include "wow_edit_drawing_bindings.h"
#include "wow_edit_lookaside_bindings.h"
#include "wow_user_registered_callbacks.h"
#include "wow_user_task_lifecycle.h"
#include "wow_user_borrow_scope.h"
#include "wow_task_profile_bindings.h"
#include "wow_default_message_bindings.h"
#include "wow_default_window_bindings.h"
#include "wow_button_lifetime.h"

__declspec(thread) wow_edit_context *wow_edit_current;

BOOL WINAPI wow_edit_capture_context(wow_edit_context *context)
{
    DWORD error = GetLastError();
    wow_edit_context value = {0};
    HDC dc;
    HGDIOBJ previous;
    BOOL measured = FALSE;
    int planes, bits;
    if (!context) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    /* Original clinit.c initializes rcScreen from a DISPLAY IC, not work
     * area or SM_CXSCREEN. Both combo placement and dialog clipping consume
     * this one scoped input; keep its coordinate contract unchanged. */
    dc = CreateICW(L"DISPLAY", NULL, NULL, NULL);
    if (!dc) return FALSE;
    value.screen_rect.right = GetDeviceCaps(dc, DESKTOPHORZRES);
    value.screen_rect.bottom = GetDeviceCaps(dc, DESKTOPVERTRES);
    planes = GetDeviceCaps(dc, PLANES);
    bits = GetDeviceCaps(dc, BITSPIXEL);
    DeleteDC(dc);
    if (value.screen_rect.right <= 0 || value.screen_rect.bottom <= 0 ||
            planes <= 0 || bits <= 0 || (UINT)planes > MAXDWORD / (UINT)bits) {
        SetLastError(ERROR_INVALID_DATA); return FALSE;
    }
    value.screen.BitCount = (UINT)planes * (UINT)bits;
    if (!SystemParametersInfoW(SPI_GETWHEELSCROLLLINES, 0,
            &value.ucWheelScrollLines, 0)) return FALSE;
    /* Original sysmet.c derives dtScroll from dtLBSearch=4*dtDblClk.
     * Do not substitute the unrelated menu delay or a fixed timer period. */
    value.dtLBSearch = GetDoubleClickTime() * 4;
    value.dtScroll = value.dtLBSearch / 5;
    /* Original NtUserGetCaretBlinkTime returns the same preference read by
     * the list's add-selection timer. Preserve the native value, including
     * INFINITE; do not invent a fixed blink interval or alter system policy. */
    value.dtCaretBlink = GetCaretBlinkTime();
    if (!value.dtCaretBlink) return FALSE;
    value.wMaxLeftOverlapChars = (int)FastGetProfileIntW(5, L"LeftOverlapChars", 3);
    value.wMaxRightOverlapChars = (int)FastGetProfileIntW(5, L"RightOverlapChars", 3);
    /* Original init.c measures SYSTEM_FONT on its bits DC through
     * GetCharDimensions. Reuse the audited GDI implementation, including
     * its average-width rounding, not tmAveCharWidth or dialog base units. */
    dc = CreateCompatibleDC(NULL);
    if (!dc) return FALSE;
    previous = SelectObject(dc, GetStockObject(SYSTEM_FONT));
    if (previous && previous != HGDI_ERROR) {
        value.cxSysFontChar = wow_edit_native_GdiGetCharDimensions(dc,
            &value.tmSysFont, &value.cySysFontChar);
        measured = value.cxSysFontChar > 0 && value.cySysFontChar > 0;
        SelectObject(dc, previous);
    }
    DeleteDC(dc);
    if (!measured) { SetLastError(ERROR_INVALID_DATA); return FALSE; }
    *context = value;
    SetLastError(error);
    return TRUE;
}

/* ADAPTER-WOW-058: original EDIT/combo/list entries, with the original /Gz ABI.
 * The editor's private A/W branch may call these while this outer entry
 * retains its scopes. External canonical dispatch uses the wrappers below.
 * The complete original control consumer/lifetime group must be linked. */
LONG WINAPI EditWndProcA(HWND, UINT, WPARAM, LONG);
LONG WINAPI EditWndProcW(HWND, UINT, WPARAM, LONG);
LONG WINAPI ComboBoxWndProcA(HWND, UINT, WPARAM, LPARAM);
LONG WINAPI ComboBoxWndProcW(HWND, UINT, WPARAM, LPARAM);
LONG WINAPI ListBoxWndProcA(HWND, UINT, WPARAM, LPARAM);
LONG WINAPI ListBoxWndProcW(HWND, UINT, WPARAM, LPARAM);
LONG WINAPI ComboListBoxWndProcA(HWND, UINT, WPARAM, LPARAM);
LONG WINAPI ComboListBoxWndProcW(HWND, UINT, WPARAM, LPARAM);
LONG WINAPI ButtonWndProcA(HWND, UINT, WPARAM, LPARAM);
LONG WINAPI ButtonWndProcW(HWND, UINT, WPARAM, LPARAM);
LONG WINAPI xxxLBoxCaretBlinker(HWND, UINT, UINT, LONG);
C_ASSERT(sizeof(LPARAM) == sizeof(LONG));

/* Original public/internal/windows/inc/winuserp.h message, not a native
 * application message or a substitute for WM_NCDESTROY. */
#define WOW_EDIT_WM_FINALDESTROY 0x0070

static BOOL memory_callbacks_ready(void)
{
    const PFNWOWHANDLERSIN *callbacks = wow_user_registered_callbacks();
    if (!callbacks->pfnLocalAlloc || !callbacks->pfnLocalReAlloc ||
            !callbacks->pfnLocalLock || !callbacks->pfnLocalUnlock ||
            !callbacks->pfnLocalSize || !callbacks->pfnLocalFree ||
            !callbacks->pfnWowEditNextWord || !callbacks->pfnEmptyCB ||
            !callbacks->pfnGetExpWinVer) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    return TRUE;
}

static LRESULT CALLBACK list_caret_entry(HWND, UINT, WPARAM, LPARAM);

/* Called only inside edit_entry's exact-WND/task/borrow scope. The inner
 * resource/private-state scope must unwind before those outer owners. */
static LRESULT CALLBACK button_bodyA(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    return wow_button_invoke(window, message, wp, lp, ButtonWndProcA);
}
static LRESULT CALLBACK button_bodyW(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    return wow_button_invoke(window, message, wp, lp, ButtonWndProcW);
}

static LRESULT edit_entry(HWND window, UINT message, WPARAM wp, LPARAM lp,
    WNDPROC procedure, BOOL finalize)
{
    DWORD entry_error = GetLastError();
    wow_user_runtime_thread *runtime = wow_user_runtime_current();
    wow_window_words_binding *words;
    PWND state;
    wow_cleanup_window *owner;
    wow_task_callback_scope task_scope;
    wow_user_borrow_scope borrow_scope;
    wow_edit_context context, *previous_context = wow_edit_current;
    wow_default_context geometry_context, *previous_geometry = wow_default_current;
    BOOL cleanup;
    BOOL needs_context = !finalize && message != WM_NCDESTROY &&
        message != WOW_EDIT_WM_FINALDESTROY;
    LRESULT result = 0;

    /* Original Local*16 callbacks need the calling WOW context even though
     * this WNDPROC is native code. Do not create/rebind a task from the HWND. */
    if (!runtime || !runtime->thread || !runtime->thread->ptdb ||
            !runtime->thread->pClientInfo) {
        SetLastError(ERROR_INVALID_STATE);
        return 0;
    }
    if (!memory_callbacks_ready() || !wow_edit_native_ready() ||
            !wow_edit_lookaside_ready() || !wow_edit_drawing_ready() ||
            !wow_default_messages_ready()) return 0;
    /* Final destruction reads no system metrics. Never make release of an
     * existing PED depend on acquiring a fresh font DC or profile snapshot. */
    if (needs_context) {
        wow_user_task_lifecycle *lifecycle = runtime->runtime ? runtime->runtime->lifecycle : NULL;
        C_ASSERT(sizeof(context.atomSysClass) == sizeof(lifecycle->cleanup_shared.atomSysClass));
        if (!lifecycle || !lifecycle->initialized ||
                runtime->thread->ppi != &lifecycle->process) {
            SetLastError(ERROR_INVALID_STATE);
            return 0;
        }
        if (!wow_edit_capture_context(&context)) return 0;
        /* Original lboxrare.c:xxxLBSize calls GetWindowBorders directly,
         * not through DefWindowProcWorker. Its gclBorder must therefore be
         * available for the whole original control invocation, including
         * resize reentry. Capture before exposing either TLS snapshot. */
        if (!wow_default_capture_context(&geometry_context, WM_NCHITTEST)) return 0;
        /* Original comboini.c consumes process-initialized class atoms.
         * Reuse the owner already populated before task admission, not
         * class names, HWND inference or a second registration table. */
        CopyMemory(context.atomSysClass, lifecycle->cleanup_shared.atomSysClass,
            sizeof(context.atomSysClass));
        if (!context.atomSysClass[ICLS_EDIT] ||
                !context.atomSysClass[ICLS_COMBOLISTBOX]) {
            SetLastError(ERROR_INVALID_STATE);
            return 0;
        }
    }
    words = wow_window_words_acquire(window);
    if (!words) return 0;
    /* ADAPTER-WOW-076: saved control procedures may be called with HWND32's
     * short carrier. Retain native validation, then publish the binding's
     * full WND identity to the strict graph/callback owners. */
    state = wow_window_words_client_state_value(words);
    if (state) window = state->head.h;
    owner = wow_window_words_cleanup_value(words);
    cleanup = (runtime->thread->TIF_flags & WOW_TASK_TIF_INCLEANUP) != 0;
    if (!owner || (cleanup && (owner->thread != runtime->thread ||
            (message != WM_NCDESTROY && message != WOW_EDIT_WM_FINALDESTROY)))) {
        wow_window_words_release(words);
        SetLastError(ERROR_INVALID_STATE);
        return 0;
    }
    if (!(cleanup ? wow_task_resource_cleanup_enter(&task_scope) :
            wow_task_callback_enter(&task_scope))) {
        DWORD error = GetLastError();
        wow_window_words_release(words);
        SetLastError(error);
        return 0;
    }
    if (!wow_user_borrow_enter(&borrow_scope)) {
        DWORD error = GetLastError();
        wow_window_words_release(words);
        (void)wow_task_callback_leave(&task_scope);
        SetLastError(error);
        return 0;
    }
    __try {
        if (needs_context) {
            wow_edit_current = &context;
            wow_default_current = &geometry_context;
        }
        SetLastError(entry_error);
        if (finalize) result = wow_control_finalize_window(window);
        else if (procedure == list_caret_entry ? wow_window_client_refresh(window) :
                wow_window_client_ensure_state(window)) {
            if (wow_window_client_prepare_graph(window) &&
                    wow_user_borrow_set_callback(window)) {
                SetLastError(entry_error);
                result = procedure(window, message, wp, lp);
            }
        }
    } __finally {
        DWORD error = GetLastError();
        wow_edit_current = previous_context;
        wow_default_current = previous_geometry;
        /* Release this entry's reference while its scopes are still live.
         * This is NOT terminal cleanup: the outer destruction owner must
         * complete original EDIT finalization before removing HWND identity.
         * A nested destroy may already have consumed that terminal owner. */
        wow_window_words_release(words);
        wow_user_borrow_leave_required(&borrow_scope);
        (void)wow_task_callback_leave(&task_scope);
        SetLastError(error);
    }
    return result;
}

LRESULT CALLBACK wow_edit_window_procA(HWND window, UINT message,
    WPARAM wp, LPARAM lp)
{
    return edit_entry(window, message, wp, lp, EditWndProcA, FALSE);
}

LRESULT CALLBACK wow_edit_window_procW(HWND window, UINT message,
    WPARAM wp, LPARAM lp)
{
    return edit_entry(window, message, wp, lp, EditWndProcW, FALSE);
}

/* All three original collaboration controls require the same task, borrow,
 * input snapshot and finally-unwind owner. Do not bypass that boundary by
 * publishing the raw original WNDPROC in a class or cleanup record. */
#define CONTROL_ENTRY(name, original) \
    static LRESULT CALLBACK name(HWND window, UINT message, WPARAM wp, LPARAM lp) \
    { return edit_entry(window, message, wp, lp, original, FALSE); }
CONTROL_ENTRY(combo_entryA, ComboBoxWndProcA)
CONTROL_ENTRY(combo_entryW, ComboBoxWndProcW)
CONTROL_ENTRY(list_entryA, ListBoxWndProcA)
CONTROL_ENTRY(list_entryW, ListBoxWndProcW)
CONTROL_ENTRY(combo_list_entryA, ComboListBoxWndProcA)
CONTROL_ENTRY(combo_list_entryW, ComboListBoxWndProcW)
CONTROL_ENTRY(button_entryA, button_bodyA)
CONTROL_ENTRY(button_entryW, button_bodyW)
#undef CONTROL_ENTRY

WNDPROC WINAPI wow_control_client_procedure(WORD fnid, BOOL unicode)
{
    switch (fnid) {
    case FNID_BUTTON: return unicode ? button_entryW : button_entryA;
    case FNID_EDIT: return unicode ? wow_edit_window_procW : wow_edit_window_procA;
    case FNID_COMBOBOX: return unicode ? combo_entryW : combo_entryA;
    case FNID_LISTBOX: return unicode ? list_entryW : list_entryA;
    case FNID_COMBOLISTBOX: return unicode ? combo_list_entryW : combo_list_entryA;
    default: return NULL;
    }
}

BOOL WINAPI wow_control_finalize_bound(HWND window)
{
    return (BOOL)edit_entry(window, WOW_EDIT_WM_FINALDESTROY, 0, 0, NULL, TRUE);
}

static LRESULT CALLBACK list_caret_entry(HWND window, UINT message,
    WPARAM id, LPARAM time)
{
    PWND state = wow_window_client_validate_hwnd(window);
    /* A queued callback must not dereference a freed original LBIV after
     * normal/final cleanup. Do not instantiate control state for a timer. */
    if (!state || state->fnid != FNID_LISTBOX) return 0;
    return xxxLBoxCaretBlinker(window, message, (UINT)id, (LONG)time);
}

static VOID CALLBACK list_caret_timer(HWND window, UINT message,
    UINT_PTR id, DWORD time)
{
    (void)edit_entry(window, message, id, (LPARAM)time, list_caret_entry, FALSE);
}

UINT WINAPI wow_control_set_timer(HWND window, UINT id, DWORD milliseconds,
    WNDPROC callback)
{
    /* ADAPTER-WOW-070: original ListBox timing and timer IDs are untouched.
     * Its sole non-NULL callback enters the same scoped owner as its WNDPROC;
     * no timer registry, guest timer or replacement blinking algorithm. */
    if (!window || (callback && callback != (WNDPROC)xxxLBoxCaretBlinker)) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    return (UINT)SetTimer(window, id, milliseconds, callback ? list_caret_timer : NULL);
}
