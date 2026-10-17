#include "wow_dialog_native_bindings.h"
#include "wow_native_text_descriptor.h"
#include <stddef.h>
#include "wow_task_order_bindings.h"

/* Current inspected x86 USER dialog calls enter a17-slot win32u service.
 * The old NtUserCreateWindowEx declaration has14 slots and is not its ABI.
 * The extra slots below are kept distinct; only the observed resolved-class,
 * no-extra-context form is used. They are NOT original WOW words.
 * Exact caller/stub evidence and group acceptance are in the S3 ledger.
 * This adapter is listed in the staged provider support graph; the original
 * dialog body/precomp group is not yet selected or verified end to end. */
typedef HWND (WINAPI *native_create_window_fn)(DWORD, const wow_native_text_descriptor *,
    const wow_native_text_descriptor *, const wow_native_text_descriptor *, DWORD, int, int, int, int, HWND, HMENU,
    HANDLE, LPVOID, ULONG_PTR, DWORD, ULONG_PTR, ULONG_PTR);
static INIT_ONCE native_once = INIT_ONCE_STATIC_INIT;
static native_create_window_fn native_create_window;
typedef VOID (WINAPI *native_zap_active_focus_fn)(VOID);
static native_zap_active_focus_fn native_zap_active_focus;
typedef VOID (WINAPI *native_set_thread_state_fn)(DWORD, DWORD);
static native_set_thread_state_fn native_set_thread_state;
typedef BOOL (WINAPI *native_set_system_menu_fn)(HWND);
static native_set_system_menu_fn native_set_system_menu;
typedef ULONG_PTR (WINAPI *native_query_window_fn)(HWND, DWORD);
typedef ULONG_PTR (WINAPI *native_get_thread_state_fn)(DWORD);
static native_query_window_fn native_query_window;
static native_get_thread_state_fn native_get_thread_state;
C_ASSERT(sizeof(void *) == 4);
C_ASSERT(sizeof(LARGE_STRING) == 12);
C_ASSERT(sizeof(LARGE_UNICODE_STRING) == sizeof(LARGE_STRING));
C_ASSERT(offsetof(LARGE_STRING, Buffer) == 8);

static BOOL CALLBACK resolve_native(PINIT_ONCE once, PVOID parameter,
    PVOID *context)
{
    HMODULE module = GetModuleHandleW(L"win32u.dll");
    native_create_window_fn create;
    native_zap_active_focus_fn zap;
    native_set_thread_state_fn set_thread_state;
    native_set_system_menu_fn set_system_menu;
    native_query_window_fn query_window;
    native_get_thread_state_fn get_thread_state;
    (void)once; (void)parameter; (void)context;
    if (!module) { SetLastError(ERROR_MOD_NOT_FOUND); return FALSE; }
    create = (native_create_window_fn)GetProcAddress(module,
        "NtUserCreateWindowEx");
    zap = (native_zap_active_focus_fn)GetProcAddress(module,
        "NtUserZapActiveAndFocus");
    set_thread_state = (native_set_thread_state_fn)GetProcAddress(module,
        "NtUserSetThreadState");
    set_system_menu = (native_set_system_menu_fn)GetProcAddress(module,
        "NtUserSetDialogSystemMenu");
    query_window = (native_query_window_fn)GetProcAddress(module, "NtUserQueryWindow");
    get_thread_state = (native_get_thread_state_fn)GetProcAddress(module, "NtUserGetThreadState");
    if (!create || !zap || !set_thread_state || !set_system_menu ||
            !query_window || !get_thread_state) {
        SetLastError(ERROR_PROC_NOT_FOUND); return FALSE;
    }
    native_create_window = create;
    native_zap_active_focus = zap;
    native_set_thread_state = set_thread_state;
    native_set_system_menu = set_system_menu;
    native_query_window = query_window;
    native_get_thread_state = get_thread_state;
    return TRUE;
}

BOOL WINAPI wow_dialog_native_ready(void)
{
    DWORD error = GetLastError();
    if (!InitOnceExecuteOnce(&native_once, resolve_native, NULL, NULL))
        return FALSE;
    SetLastError(error);
    return TRUE;
}

BOOL WINAPI wow_dialog_native_zap_active_and_focus(void)
{
    if (!wow_dialog_native_ready()) return FALSE;
    /* ADAPTER-WOW-062: original EndDialog's no-notification queue reset.
     * Current USER calls this named zero-argument export at the same edge.
     * Do not substitute the activating/notifying SetActiveWindow/SetFocus
     * pair or reuse an NT4 NtUserCallNoParam operation number. */
    native_zap_active_focus();
    return TRUE; /* Resolution succeeded; the original operation is VOID. */
}

BOOL WINAPI wow_dialog_current_foreground(void)
{
    if (!wow_dialog_native_ready())
        wow_user_native_call_failed("dialog-current-foreground", GetLastError());
    /* Verified x86 USER DialogBox2 at10089890, not NT4 enum12. */
    return (BOOL)native_get_thread_state(10);
}

POINT WINAPI wow_dialog_cursor_position(void)
{
    DWORD error = GetLastError();
    POINT point;
    /* DS_CENTERMOUSE reads at the placement decision, after earlier
     * creation callbacks. Do not cache the cursor at outer entry. */
    if (!GetCursorPos(&point))
        wow_user_native_call_failed("dialog-cursor-position", GetLastError());
    SetLastError(error);
    return point;
}

BOOL WINAPI wow_dialog_window_foreground(HWND window)
{
    if (!wow_dialog_native_ready())
        wow_user_native_call_failed("dialog-window-foreground", GetLastError());
    /* Same modal-return conditional at1008989C, not NT4 enum6. */
    return (BOOL)native_query_window(window, 7);
}

BOOL WINAPI wow_dialog_native_set_thread_state(DWORD flags, DWORD mask)
{
    if (!wow_dialog_native_ready()) return FALSE;
    native_set_thread_state(flags, mask);
    return TRUE; /* Dependency availability, not a fabricated native result. */
}

BOOL WINAPI wow_dialog_native_set_system_menu(HWND window)
{
    if (!wow_dialog_native_ready()) return FALSE;
    return native_set_system_menu(window);
}

HWND WINAPI wow_dialog_native_create_window(DWORD ex_style, ATOM class_atom,
    const LARGE_STRING *title, DWORD style, int x, int y, int width, int height,
    HWND parent, HMENU menu, HANDLE module, LPVOID parameter, DWORD version)
{
    const wow_native_text_descriptor *atom;
    wow_native_text_descriptor native_title;
    if (!class_atom) { SetLastError(ERROR_INVALID_PARAMETER); return NULL; }
    if (!wow_dialog_native_ready()) return NULL;
    atom = (const wow_native_text_descriptor *)(ULONG_PTR)class_atom;
    if (title) wow_native_text_capture(&native_title, title);
    /* No text conversion: bAnsi describes the counted title, independently
     * of ex_style's original WS_EX_ANSICREATOR. A resolved class atom needs
     * no string redirection or captured activation context. Original
     * createw.c stores LOWORD(version) in WND; full WOW facts stay upstream.
     * Native slots14/16/17 are zero as in the inspected dialog call form.
     * Resolve no internal function address and introduce no syscall number. */
    return native_create_window(ex_style, atom, atom, title ? &native_title : NULL,
        style, x, y, width, height, parent, menu, module, parameter,
        0, (DWORD)LOWORD(version), 0, 0);
}
