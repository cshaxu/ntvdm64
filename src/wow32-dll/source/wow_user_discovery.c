#include "wow_user_private_access.h"
#include "wow_user_object_bindings.h"
#include "wow_user_runtime.h"
#include "wow_user_task_lifecycle.h"

/* Native USER owns discovery; original WOW owns conversion and callback
 * execution. Only publish the discovered full identities before USER16 can
 * read them. Enumeration's HWND does not select another execution owner. */
static BOOL publish_discovery(HWND window)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    BOOL held, result = FALSE;
    DWORD error = ERROR_SUCCESS;
    if (!binding) return wow_user_window_refresh_graph(window);
    held = binding->exclusive_held;
    if (!held && !wow_user_runtime_enter(binding)) return FALSE;
    __try {
        /* Original HWND32 deliberately sign-extends a USER16 handle. Native
         * EnumProps echoes that input to its callback (unlike EnumWindows,
         * which supplies full handles). Recover its existing shared-table
         * identity with retained GetFullUserHandle before publishing a WND;
         * a short carrier is not a new zero/FFFF generation. */
        if (window && ((ULONG)(ULONG_PTR)window == LOWORD(window) ||
                (LONG)(LONG_PTR)window == (LONG)(SHORT)LOWORD(window))) {
            if (binding->thread && binding->thread->ppi && binding->thread->ppi->objects) {
                DWORD full = wow_user_full_handle(binding->thread->ppi->objects, LOWORD(window));
                if (!full) { error = GetLastError(); __leave; }
                window = (HWND)(ULONG_PTR)full;
            }
        }
        result = wow_user_window_refresh_graph(window);
        error = GetLastError();
    } __finally {
        if (!held && !wow_user_runtime_leave(binding)) {
            result = FALSE;
            error = GetLastError();
        }
    }
    SetLastError(error);
    return result;
}

static HWND discovered(HWND window)
{
    DWORD error = GetLastError();
    if (window && !publish_discovery(window)) return NULL;
    SetLastError(error);
    return window;
}

#define DISCOVERY(name, parameters, arguments) \
HWND WINAPI wow_native_##name parameters { return discovered(name arguments); }
DISCOVERY(FindWindowA, (LPCSTR cls, LPCSTR title), (cls, title))
DISCOVERY(ChildWindowFromPoint, (HWND parent, POINT point), (parent, point))
DISCOVERY(WindowFromPoint, (POINT point), (point))
DISCOVERY(GetCapture, (void), ())
DISCOVERY(GetFocus, (void), ())
DISCOVERY(GetActiveWindow, (void), ())
DISCOVERY(GetForegroundWindow, (void), ())
DISCOVERY(GetLastActivePopup, (HWND owner), (owner))
DISCOVERY(GetNextDlgGroupItem, (HWND dialog, HWND control, BOOL previous), (dialog, control, previous))
DISCOVERY(GetNextDlgTabItem, (HWND dialog, HWND control, BOOL previous), (dialog, control, previous))
#undef DISCOVERY

LONG WINAPI wow_native_GetWindowLongA(HWND window, int index)
{
    LONG result = GetWindowLongA(window, index);
    /* getset.c selects owner versus parent; all other LONGs stay numeric. */
    return index == GWL_HWNDPARENT ? (LONG)discovered((HWND)result) : result;
}

typedef struct discovery_enumeration {
    WNDENUMPROC callback;
    LPARAM parameter;
    BOOL failed;
    DWORD error;
    BOOL invoked;
} discovery_enumeration;

static BOOL CALLBACK discovered_callback(HWND window, LPARAM parameter)
{
    discovery_enumeration *scope = (discovery_enumeration *)parameter;
    DWORD error = GetLastError();
    if (!publish_discovery(window)) {
        scope->failed = TRUE;
        scope->error = GetLastError();
        return FALSE;
    }
    SetLastError(error);
    scope->invoked = TRUE;
    return scope->callback(window, scope->parameter);
}

static BOOL enumeration_return(discovery_enumeration *scope, BOOL result)
{
    DWORD error = GetLastError();
    if (scope->failed) {
        SetLastError(scope->error);
        return FALSE;
    }
    if (!result && !scope->invoked) return result;
    if (!publish_discovery(NULL)) return FALSE;
    SetLastError(error);
    return result;
}

/* Release only caller-held USER data protection, never WOW execution.
 * Native enumeration and arbitrary nested callbacks run outside that lock.
 * The existing callout pair restores the entry state even on an exception. */
#define ENUM_DISCOVERY(name, parameters, native_args, wrapped_args) \
BOOL WINAPI wow_native_##name parameters \
{ \
    discovery_enumeration scope = {callback, parameter, FALSE, 0, FALSE}; \
    wow_user_runtime_thread *binding = wow_user_runtime_current(); \
    BOOL held = binding && binding->exclusive_held, result = FALSE; \
    DWORD error = GetLastError(); \
    if (held && !wow_user_runtime_leave(binding)) return FALSE; \
    SetLastError(error); \
    __try { \
        result = callback ? name wrapped_args : name native_args; \
        if (callback) result = enumeration_return(&scope, result); \
    } __finally { \
        error = GetLastError(); \
        if (held && !wow_user_runtime_enter(binding)) result = FALSE; \
        else SetLastError(error); \
    } \
    return result; \
}
ENUM_DISCOVERY(EnumWindows, (WNDENUMPROC callback, LPARAM parameter),
    (callback, parameter), (discovered_callback, (LPARAM)&scope))
ENUM_DISCOVERY(EnumChildWindows, (HWND parent, WNDENUMPROC callback, LPARAM parameter),
    (parent, callback, parameter), (parent, discovered_callback, (LPARAM)&scope))
ENUM_DISCOVERY(EnumThreadWindows, (DWORD thread, WNDENUMPROC callback, LPARAM parameter),
    (thread, callback, parameter), (thread, discovered_callback, (LPARAM)&scope))
#undef ENUM_DISCOVERY

typedef struct property_enumeration {
    PROPENUMPROCA callback;
    BOOL failed;
    BOOL invoked;
    DWORD error;
} property_enumeration;

static int CALLBACK discovered_property(HWND window, LPSTR name, HANDLE data,
    ULONG_PTR parameter)
{
    property_enumeration *scope = (property_enumeration *)parameter;
    DWORD error = GetLastError();
    if (!publish_discovery(window)) {
        scope->failed = TRUE;
        scope->error = GetLastError();
        return 0;
    }
    SetLastError(error);
    scope->invoked = TRUE;
    return scope->callback(window, name, data);
}

int WINAPI wow_native_EnumPropsA(HWND window, PROPENUMPROCA callback)
{
    property_enumeration scope = {callback, FALSE, FALSE, 0};
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    BOOL held = binding && binding->exclusive_held;
    int result = -1;
    DWORD error = GetLastError();
    if (held && !wow_user_runtime_leave(binding)) return -1;
    SetLastError(error);
    __try {
        /* Original clenum.c uses the same InternalEnumProps for A and ExA;
         * Ex only carries this stack context. Keep native enumeration and
         * the original W32EnumPropsFunc atom/string/guest conversion. */
        result = callback ? EnumPropsExA(window, discovered_property,
            (LPARAM)&scope) : EnumPropsA(window, callback);
        error = GetLastError();
        if (scope.invoked && !publish_discovery(NULL))
            wow_user_native_call_failed("publish-property-return", GetLastError());
        if (scope.failed) { result = -1; error = scope.error; }
        SetLastError(error);
    } __finally {
        error = GetLastError();
        if (held && !wow_user_runtime_enter(binding))
            wow_user_native_call_failed("restore-property-owner", GetLastError());
        SetLastError(error);
    }
    return result;
}
