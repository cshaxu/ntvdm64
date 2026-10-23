#include "wow_default_window_bindings.h"
#include "wow_user_private_access.h"
#include "wow_user_borrow_scope.h"
#include "wow_user_task_lifecycle.h"
#include "wow_user_client_view_layout.h"

/* ADAPTER-WOW-063: approved nonclient boundary. Both original FindNCHit
 * callers reach one owner; never SendMessage back into the same HWND.
 * The modern host owns nonclient geometry and hit testing. */
int WINAPI wow_default_find_nc_hit(PWND state, LONG point)
{
    wow_user_native_call call;
    DWORD error=GetLastError();
    HWND window;
    LRESULT result=HTERROR;
    if(!state){SetLastError(ERROR_INVALID_WINDOW_HANDLE);return HTERROR;}
    window=state->head.h;
    if(!wow_user_borrow_client_state(window,state))return HTERROR;
    if(!wow_user_native_call_begin(&call,window))return HTERROR;
    __try {
        SetLastError(error);
        result=DefWindowProcW(window,WM_NCHITTEST,0,(LPARAM)point);
    } __finally {
        error=GetLastError();
        if(!wow_user_native_call_end(&call))
            wow_user_native_call_failed("nonclient-hit-return",GetLastError());
        SetLastError(error);
    }
    return (int)result;
}

/* ADAPTER-WOW-061: the original client's explicit server operations use
 * the existing native boundary, including execution restoration and return
 * state publication. The client's own algorithm remains original clmsg.c.
 * Never dispatch a private PWND as an HWND or reinterpret arbitrary FNIDs. */
LONG WINAPI wow_default_CsSendMessage(HWND window, UINT message, WPARAM wp,
    LPARAM lp, DWORD extra, DWORD function, BOOL ansi)
{
    if (function != FNID_DEFWINDOWPROC || extra != 0) {
        SetLastError(ERROR_NOT_SUPPORTED);
        return 0;
    }
    return (LONG)(ansi ? wow_native_DefWindowProcA(window, message, wp, lp) :
        wow_native_DefWindowProcW(window, message, wp, lp));
}
