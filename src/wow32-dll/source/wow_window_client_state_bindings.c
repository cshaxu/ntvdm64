#include "wow_window_client_state_bindings.h"
#include "wow_user_client_view_layout.h"
#include "wow_user_borrow_scope.h"
#include "wow_user_runtime.h"
#include "wow_user_object_bindings.h"
#include "wow_task_order_bindings.h"
#include "wow_edit_native_bindings.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h"
#include "opennt-abi/source/public/internal/windows/inc/wingdip.h"

/* One host WND/class-extra allocation per existing HWND binding. Original
 * controls/dialogs own their interpretation and private-state lifecycle. */
C_ASSERT(sizeof(void *) == 4);

BOOL WINAPI wow_window_client_capture_name(HWND window, WCHAR **text,
    ULONG *length, ULONG *capacity)
{
    DWORD saved = GetLastError(), error;
    ULONG chars = 256;
    WCHAR *buffer;
    int copied;
    if (!text || !length || !capacity) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    for (;;) {
        buffer = HeapAlloc(GetProcessHeap(), 0, chars * sizeof(WCHAR));
        if (!buffer) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE; }
        SetLastError(ERROR_SUCCESS);
        copied = InternalGetWindowText(window, buffer, (int)chars);
        error = GetLastError();
        if (copied < 0 || (ULONG)copied >= chars ||
                (!copied && error != ERROR_SUCCESS) || !IsWindow(window)) {
            HeapFree(GetProcessHeap(), 0, buffer);
            SetLastError(error ? error : ERROR_INVALID_WINDOW_HANDLE);
            return FALSE;
        }
        if ((ULONG)copied < chars - 1) {
            buffer[copied] = L'\0';
            *text = buffer;
            *length = (ULONG)copied * sizeof(WCHAR);
            *capacity = chars * sizeof(WCHAR);
            SetLastError(saved); return TRUE;
        }
        HeapFree(GetProcessHeap(), 0, buffer);
        /* LARGE_UNICODE_STRING.MaximumLength has 31 bits. A full buffer is
         * retried, never accepted as a silently truncated stored name. */
        if (chars > 0x3fffffffUL / 2) {
            SetLastError(ERROR_ARITHMETIC_OVERFLOW); return FALSE;
        }
        chars *= 2;
    }
}

BOOL WINAPI wow_window_client_ensure_state(HWND window)
{
    DWORD error = GetLastError();
    wow_window_words_binding *binding = wow_user_borrow_enrolled_window(window);
    wow_window_creation_facts facts;
    if (!binding) return FALSE;
    if (wow_window_words_owns_client_state(binding,
            wow_window_words_client_state_value(binding))) {
        /* Canonical native EDIT/dialog entries do not pass through the WOW
         * procedure branch that refreshes WW before DispatchClientMessage.
         * Reuse that same update owner before each ordinary original entry;
         * existence alone does not make the creation-time WND view current.
         * Final destruction deliberately does not use ensure_state. */
        if (!wow_window_words_refresh(window, binding)) return FALSE;
        SetLastError(error); return TRUE;
    }
    if (!wow_window_words_creation_facts(binding, &facts)) return FALSE;
    return wow_window_client_create_state(window, facts.module_version,
        facts.compatibility_flags, facts.class_extra_bytes);
}

BOOL WINAPI wow_window_client_create_state(HWND window,
    DWORD module_version, DWORD compatibility_flags, int class_extra_bytes)
{
    DWORD error = GetLastError();
    wow_window_words_binding *binding;
    PWND state;
    WW *words;
    WINDOWINFO info;
    BOOL ansi_creator;
    DWORD control_id = 0;
    ULONG name_length, name_capacity;
    WCHAR *name;
    SIZE_T allocation_bytes;
    if (class_extra_bytes < 0) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    binding = wow_user_borrow_enrolled_window(window);
    if (!binding) return FALSE;
    if (wow_window_words_owns_client_state(binding,
            wow_window_words_client_state_value(binding))) {
        SetLastError(ERROR_ALREADY_EXISTS);
        return FALSE;
    }
    if (!wow_window_words_creation_ansi(binding, &ansi_creator)) return FALSE;
    /* Reuse the ID accepted by the same refresh as styles/geometry; querying
     * it again could mix samples. Non-child IDs are zero, never PMENU values. */
    if (!wow_window_words_refresh_info(window, binding, &info, &control_id))
        return FALSE;
    /* Original createw.c allocates WND plus the selected class's extra
     * extent, not merely the canonical control structure. A superclass may
     * reserve more than the PED word. Keep that declared extent in this
     * same retained allocation; no separate extra-byte/dialog allocation.
     * The minimum extra word preserves safe pre-validation control access
     * without changing cbwndExtra checked by original control code. */
    allocation_bytes = sizeof(WND) + (SIZE_T)class_extra_bytes;
    if (allocation_bytes < sizeof(WND) + sizeof(PVOID))
        allocation_bytes = sizeof(WND) + sizeof(PVOID);
    state = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, allocation_bytes);
    if (!state) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return FALSE;
    }
    if (!wow_window_client_capture_name(window, &name, &name_length, &name_capacity)) {
        DWORD failure = GetLastError();
        HeapFree(GetProcessHeap(), 0, state);
        SetLastError(failure); return FALSE;
    }
    state->strName.Buffer = name;
    state->strName.Length = name_length;
    state->strName.MaximumLength = name_capacity;
    state->strName.bAnsi = FALSE;
    words = wow_window_words_value(binding);
    state->head.h = window;
    state->head.pSelf = (PBYTE)state;
    state->cbwndExtra = class_extra_bytes;
    state->rcWindow = info.rcWindow;
    state->rcClient = info.rcClient;
    CopyMemory(state->adwWOW, words->adwWOW, sizeof(state->adwWOW));
    state->style = words->dwStyle;
    state->ExStyle = words->dwExStyle;
    state->hModule = words->hInstance;
    state->spmenu = (PMENU)(ULONG_PTR)(DWORD)control_id;
    /* ADAPTER-WOW-063: shared creation-state projection from original createw.c
     * (ANSI transfer and lines 354--371 compatibility branches). The full
     * server creator cannot be composed; its caller provides these exact
     * source inputs. Native creation/style policy is not implemented here. */
    if (ansi_creator) SetWF(state, WFANSICREATOR);
    if (compatibility_flags & GACF_RANDOM3XUI) SetWF(state, WFOLDUI);
    state->dwExpWinVer = (DWORD)LOWORD(module_version);
    if (Is310Compat(state->dwExpWinVer)) {
        SetWF(state, WFWIN31COMPAT);
        if (Is400Compat(state->dwExpWinVer))
            SetWF(state, WFWIN40COMPAT);
    } else if (compatibility_flags & GACF_ALWAYSSENDNCPAINT)
        SetWF(state, WFALWAYSSENDNCPAINT);
    if (!wow_window_words_attach_client_state(binding, state)) {
        error = GetLastError();
        HeapFree(GetProcessHeap(), 0, name);
        HeapFree(GetProcessHeap(), 0, state);
        SetLastError(error);
        return FALSE;
    }
    SetLastError(error);
    return TRUE;
}

PWND WINAPI wow_window_client_validate_hwnd(HWND window)
{
    DWORD error = GetLastError();
    wow_window_words_binding *binding = wow_user_borrow_enrolled_window(window);
    PWND state;
    if (!binding) return NULL;
    state = wow_window_words_client_state_value(binding);
    if (!wow_window_words_owns_client_state(binding, state)) {
        SetLastError(ERROR_INVALID_STATE);
        return NULL;
    }
    SetLastError(error);
    return state;
}

PWND WINAPI wow_window_client_validate_window(HWND window)
{
    DWORD saved = GetLastError(), error;
    wow_user_runtime_thread *runtime = wow_user_runtime_current();
    PWND state = NULL;
    BOOL held;
    if (!runtime) { SetLastError(ERROR_INVALID_STATE); return NULL; }
    held = runtime->exclusive_held;
    if (!held && !wow_user_runtime_enter(runtime)) return NULL;
    __try {
        /* client/wow.c callback-first identity, then rtl/wow.c handle
         * validation, over the existing retained projection rather than
         * NT4's unavailable shared USER table/desktop address mapping. */
        state = wow_user_borrow_window_state(window);
    } __finally {
        error = state ? saved : GetLastError();
        if (!held && !wow_user_runtime_leave(runtime))
            wow_user_native_call_failed("release-window-validation", GetLastError());
        SetLastError(error);
    }
    return state;
}

PWND WINAPI wow_window_client_desktop(void)
{
    /* client/wow.c returns the current desktop WND. Resolve its public
     * identity through the same retained domain, not NT4 desktop memory. */
    if (!wow_edit_native_ensure_gui_thread()) return NULL;
    return wow_window_client_validate_window(GetDesktopWindow());
}

DWORD WINAPI wow_window_client_context_help(PWND state)
{
    DWORD saved = GetLastError(), result = 0;
    wow_user_runtime_thread *runtime = wow_user_runtime_current();
    wow_window_words_binding *binding;
    mvdm_softpc_wow_window_observation view;
    ULONG backing = 0;
    BOOL held;
    if (!runtime || !state)
        wow_user_native_call_failed("context-help-owner", ERROR_INVALID_STATE);
    held = runtime->exclusive_held;
    if (!held && !wow_user_runtime_enter(runtime))
        wow_user_native_call_failed("context-help-enter", GetLastError());
    __try {
        binding = wow_user_borrow_client_state(state->head.h, state);
        if (!binding || !wow_window_words_pin_generation(binding, &backing) ||
                !backing || !mvdm_softpc_wow_page_domain_read_window_view(backing, &view))
            wow_user_native_call_failed("context-help-generation", ERROR_INVALID_STATE);
        result = view.context_help_id;
    } __finally {
        if (backing && !mvdm_softpc_wow_page_domain_release_window(backing))
            wow_user_native_call_failed("context-help-release", ERROR_INVALID_STATE);
        if (!held && !wow_user_runtime_leave(runtime))
            wow_user_native_call_failed("context-help-leave", GetLastError());
        SetLastError(saved);
    }
    return result;
}

PWND WINAPI wow_window_client_validate_window_no_rip(HWND window)
{
    DWORD saved = GetLastError();
    PWND state = wow_window_client_validate_window(window);
    /* The original NoRip omits HMValidateHandle's invalid-handle RIPERR.
     * Do not disguise a failed adapter allocation/owner invariant as that
     * ordinary invalid-handle result. Both names share the same resolver. */
    if (!state && GetLastError() == ERROR_INVALID_WINDOW_HANDLE)
        SetLastError(saved);
    return state;
}

BOOL WINAPI wow_window_client_refresh(HWND window)
{
    DWORD error = GetLastError();
    wow_window_words_binding *binding = wow_user_borrow_enrolled_window(window);
    if (!binding) return FALSE;
    if (!wow_window_words_owns_client_state(binding,
            wow_window_words_client_state_value(binding))) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    if (!wow_window_words_refresh(window, binding)) return FALSE;
    SetLastError(error);
    return TRUE;
}

BOOL WINAPI wow_window_client_owns_state(PWND state)
{
    DWORD error = GetLastError();
    wow_window_words_binding *binding = state ?
        wow_user_borrow_client_state(state->head.h, state) : NULL;
    BOOL owned = wow_window_words_owns_client_state(binding, state);
    /* ADAPTER-WOW-063: attach_client_state accepts only an enrolled local
     * binding. An exact scoped alias therefore retains that provenance even
     * after retirement; re-querying the HWND would confuse validity with
     * ownership, or select a replacement's property. The explicit capability
     * check keeps resource-only WND projection distinct. The lookup rejects
     * copied WNDs and uses normal process validation for new acquisitions.
     * This does not revive an HWND: ValidateHwnd remains a separate check.
     * Original callers own their denial error.
     * This is not a general replacement for NT4 FCallerOk's system/logon
     * privilege checks; those objects cannot enter this local owner. */
    SetLastError(error);
    return owned;
}

BOOL WINAPI wow_window_client_prepare_graph(HWND window)
{
    DWORD saved = GetLastError(), error;
    wow_user_runtime_thread *runtime = wow_user_runtime_current();
    wow_window_words_binding *binding;
    BOOL held, result = FALSE;
    if (!runtime) { SetLastError(ERROR_INVALID_STATE); return FALSE; }
    held = runtime->exclusive_held;
    if (!held && !wow_user_runtime_enter(runtime)) return FALSE;
    __try {
        binding = wow_user_borrow_enrolled_window(window);
        if (!binding) __leave;
        if (!wow_user_window_refresh_graph(window)) __leave;
        result = wow_user_borrow_client_graph(binding, GetDesktopWindow());
    } __finally {
        error = result ? saved : GetLastError();
        if (!held && !wow_user_runtime_leave(runtime))
            wow_user_native_call_failed("release-original-graph-entry", GetLastError());
        SetLastError(error);
    }
    return result;
}
