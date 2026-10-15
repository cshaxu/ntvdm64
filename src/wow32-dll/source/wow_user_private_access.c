#include "wow_user_private_access.h"
#include "wow_user_borrow_scope.h"
#include "wow_user_runtime.h"
#include "wow_class_client_bindings.h"
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
        DWORD error = GetLastError();
        DWORD procedure;
        BOOL bound;
        /* Original GetWindowData returns the client procedure, not a worker
         * transport entry. Native subclass carriers (including NTW16S's
         * comctl32 chain) can sit above that entry without owning the WOW
         * target. Do not expose them as the guest's previous procedure. */
        bound = wow_window_procedure_read(window, &procedure);
        if (bound && procedure)
            return (LONG)procedure;
        if (native == (LONG)wow_window_native_proc) {
            if (bound) SetLastError(ERROR_INVALID_DATA);
            return 0;
        }
        SetLastError(error);
        return native;
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
    DWORD previous_word;
    if (index == GWL_WNDPROC) {
        DWORD previous, current, error = GetLastError();
        BOOL bound;
        /* Keep the native entrance installed while the original WOW owner
         * switches its logical target and subsequently updates the WW words.
         * This also keeps any native subclasses outside it intact. */
        bound = wow_window_procedure_read(window, &current);
        if (bound && current) {
            if (!wow_window_procedure_exchange(window, (DWORD)value, &previous)) return 0;
            return (LONG)previous;
        }
        if (GetWindowLongA(window, index) == (LONG)wow_window_native_proc) {
            if (bound) SetLastError(ERROR_INVALID_DATA);
            return 0;
        }
        /* A missing owner is not permission to install a guest far address
         * as a native callback. Real native CPD values remain USER-owned. */
        if (HIWORD(value) != WNDPROC_HANDLE && ((DWORD)value & WNDPROC_WOW)) {
            SetLastError(ERROR_INVALID_STATE); return 0;
        }
        SetLastError(error);
        return wow_native_SetWindowLongA(window, index, value);
    }
    if (index > GWL_WOWDWORD1 || index < GWL_WOWDWORD3)
        return wow_native_SetWindowLongA(window, index, value);
    /* Original private branches store the word but leave dwOld uninitialized.
     * Selected SETWL callers discard it. Return the actual previous word,
     * never reproduce an indeterminate native return register. */
    if (!wow_user_borrow_window_exchange(window, GWL_WOWDWORD1 - index,
            (DWORD)value, &previous_word)) return 0;
    return (LONG)previous_word;
}

DWORD WINAPI wow_user_get_class_longA(HWND window, int index)
{
    if (index == GCL_WNDPROC) {
        DWORD native = GetClassLongA(window, index), procedure;
        DWORD error = GetLastError();
        if (wow_window_class_procedure_read(window, &procedure)) return procedure;
        if (native == (DWORD)wow_window_native_proc) return 0;
        SetLastError(error);
        return native;
    }
    if (index != GCL_WOWWORDS) return GetClassLongA(window, index);
    return (DWORD)wow_user_borrow_window_class(window);
}

DWORD WINAPI wow_user_set_class_longA(HWND window, int index, LONG value)
{
    DWORD *words;
    if (index == GCL_WNDPROC) {
        DWORD previous, current, error = GetLastError();
        if (wow_window_class_procedure_read(window, &current)) {
            if (!wow_window_class_procedure_exchange(window, (DWORD)value, &previous)) return 0;
            return previous;
        }
        if (GetClassLongA(window, index) == (DWORD)wow_window_native_proc)
            return 0;
        if (HIWORD(value) != WNDPROC_HANDLE && ((DWORD)value & WNDPROC_WOW)) {
            SetLastError(ERROR_INVALID_STATE); return 0;
        }
        SetLastError(error);
        return SetClassLongA(window, index, value);
    }
    if (index != GCL_WOWDWORD1 && index != GCL_WOWDWORD2)
        return SetClassLongA(window, index, value);
    words = wow_user_borrow_window_class(window);
    if (!words) return 0;
    /* Same deterministic previous-value boundary as the private WW setter. */
    return (DWORD)InterlockedExchange((LONG *)&words[GCL_WOWDWORD1 - index], value);
}

BOOL WINAPI wow_user_get_class_infoA(HINSTANCE module, LPCSTR name, LPWNDCLASSA output)
{
    wow_user_runtime_thread *runtime = wow_user_runtime_current();
    return wow_class_client_query(runtime ? runtime->classes : NULL,
        module, name, output);
}
