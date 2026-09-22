#ifndef WOW_WINDOW_CREATION_BINDING_H
#define WOW_WINDOW_CREATION_BINDING_H
#include "wow_window_words_binding.h"
#include "wow_task_order_bindings.h"

/* Native creation boundary after original client policy/conversion. words
 * is a complete WW, NOT the original 12-byte lpWOW input. The composition
 * owner initializes that view and serializes class metadata access. The
 * native class procedure must detach WW on WM_NCDESTROY; the supplied
 * wow_window_native_proc performs this cleanup. No GUI lock may
 * remain held across this call's reentrant callbacks. A NULL target inherits
 * the retained class target when present, as original createw.c does. */
/* The caller supplies the actual current USER thread and retains it while
 * its windows are published. Its original process owns the class roots. */
HWND WINAPI wow_window_create_boundA(wow_task_order_thread *, ATOM,
    const CREATESTRUCTA *, const struct _WW *, const wow_window_dispatch_target *);
HWND WINAPI wow_window_create_boundW(wow_task_order_thread *, ATOM,
    const CREATESTRUCTW *, const struct _WW *, const wow_window_dispatch_target *);
/* Exact PFNCSCREATEWINDOWEX carrier used by the selected WOW32 `wuwind.c`
 * caller.  That caller is ANSI-only and supplies the original 12-byte WW
 * input prefix.  This adapter copies only that prefix, then delegates native
 * creation/callback ordering to the bounded gateway above. */
HWND WINAPI wow_user_create_window_ex(DWORD, LPCTSTR, LPCTSTR, DWORD, int,
    int, int, int, HWND, HMENU, HANDLE, LPVOID, DWORD, LPDWORD);
#endif
