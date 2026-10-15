#include "wow_user_private_access.h"
#include "wow_user_object_bindings.h"
#include "wow_user_runtime.h"

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
