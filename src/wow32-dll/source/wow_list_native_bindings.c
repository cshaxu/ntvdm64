#include "wow_list_native_bindings.h"

/* Original lboxctl2.c -> _ResetDblClk has no arguments and no callback.
 * The modern named syscall returns BOOL; do not reuse NT4's SFI number.
 * This is an x86 WOW32 leaf, not a private WND/queue layout reader. */
typedef BOOL (WINAPI *reset_double_click_fn)(void);
static INIT_ONCE reset_once = INIT_ONCE_STATIC_INIT;
static reset_double_click_fn reset_double_click;

static BOOL CALLBACK resolve_reset(PINIT_ONCE once, PVOID parameter,
    PVOID *context)
{
    HMODULE module = GetModuleHandleW(L"win32u.dll");
    reset_double_click_fn candidate;
    (void)once; (void)parameter; (void)context;
    if (!module) {
        SetLastError(ERROR_MOD_NOT_FOUND);
        return FALSE;
    }
    candidate = (reset_double_click_fn)GetProcAddress(module,
        "NtUserResetDblClk");
    if (!candidate) {
        SetLastError(ERROR_PROC_NOT_FOUND);
        return FALSE;
    }
    reset_double_click = candidate;
    return TRUE;
}

BOOL WINAPI wow_list_native_reset_double_click(void)
{
    DWORD error = GetLastError();
    if (!InitOnceExecuteOnce(&reset_once, resolve_reset, NULL, NULL))
        return FALSE;
    SetLastError(error);
    return reset_double_click();
}
