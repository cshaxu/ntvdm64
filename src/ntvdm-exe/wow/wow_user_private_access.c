#include "wow_user_private_access.h"
#include "wow_user_borrow_scope.h"
#include "opennt-abi/host-compat/include/wow32_provider_private.h"
typedef VOID (APIENTRY *PFNW32ET)(VOID);
#pragma warning(push)
#pragma warning(disable:4201)
#include "opennt-abi/source/public/internal/windows/inc/wowuserp.h"
#pragma warning(pop)

/* ADAPTER-WOW-033: bind original getset.c/classc.c private indices to owned
 * WW/WC aliases and reveal the logical target behind our native gateway.
 * Native USER retains public handle/process and CPD conversion checks.
 * A selected thunk or
 * callback must own the enclosing alias scope. Original class clones share
 * these same two WC words rather than independently copied adapter state. */
LONG WINAPI wow_user_get_window_longA(HWND window, int index)
{
    PWW words;
    if (index == GWL_WNDPROC) {
        LONG native = GetWindowLongA(window, index);
        DWORD procedure;
        if (native != (LONG)wow_window_native_proc) return native;
        /* Original GetWindowData returns the client procedure, not a worker
         * transport entry. Only our ANSI gateway needs this substitution. */
        if (!wow_window_procedure_read(window, &procedure)) return 0;
        if (!procedure) { SetLastError(ERROR_INVALID_DATA); return 0; }
        return (LONG)procedure;
    }
    if (index != GWL_WOWWORDS &&
            (index > GWL_WOWDWORD1 || index < GWL_WOWDWORD3))
        return GetWindowLongA(window, index);
    words = wow_user_borrow_window(window);
    if (!words) return 0;
    if (index == GWL_WOWWORDS) return (LONG)words;
    return (LONG)words->adwWOW[GWL_WOWDWORD1 - index];
}

LONG WINAPI wow_user_set_window_longA(HWND window, int index, LONG value)
{
    PWW words;
    if (index == GWL_WNDPROC &&
            GetWindowLongA(window, index) == (LONG)wow_window_native_proc) {
        DWORD previous;
        /* Keep the native entrance installed while the original WOW owner
         * switches its logical target and subsequently updates the WW words.
         * CPD conversion remains above the raw storage contract. */
        if (!wow_window_procedure_exchange(window, (DWORD)value, &previous)) return 0;
        return (LONG)previous;
    }
    if (index > GWL_WOWDWORD1 || index < GWL_WOWDWORD3)
        return SetWindowLongA(window, index, value);
    words = wow_user_borrow_window(window);
    if (!words) return 0;
    /* Original private branches store the word but leave dwOld uninitialized.
     * Selected SETWL callers discard it. Return the actual previous word,
     * never reproduce an indeterminate native return register. */
    return InterlockedExchange((LONG *)&words->adwWOW[GWL_WOWDWORD1 - index], value);
}

DWORD WINAPI wow_user_get_class_longA(HWND window, int index)
{
    if (index == GCL_WNDPROC) {
        DWORD native = GetClassLongA(window, index), procedure;
        if (native != (DWORD)wow_window_native_proc) return native;
        if (!wow_window_class_procedure_read(window, &procedure)) return 0;
        return procedure;
    }
    if (index != GCL_WOWWORDS) return GetClassLongA(window, index);
    return (DWORD)wow_user_borrow_window_class(window);
}

DWORD WINAPI wow_user_set_class_longA(HWND window, int index, LONG value)
{
    DWORD *words;
    if (index == GCL_WNDPROC &&
            GetClassLongA(window, index) == (DWORD)wow_window_native_proc) {
        DWORD previous;
        if (!wow_window_class_procedure_exchange(window, (DWORD)value, &previous)) return 0;
        return previous;
    }
    if (index != GCL_WOWDWORD1 && index != GCL_WOWDWORD2)
        return SetClassLongA(window, index, value);
    words = wow_user_borrow_window_class(window);
    if (!words) return 0;
    /* Same deterministic previous-value boundary as the private WW setter. */
    return (DWORD)InterlockedExchange((LONG *)&words[GCL_WOWDWORD1 - index], value);
}
