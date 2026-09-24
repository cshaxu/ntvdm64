#ifndef WOW_WINDOW_DISPATCH_BINDINGS_H
#define WOW_WINDOW_DISPATCH_BINDINGS_H
#include <windows.h>

/* Exact PFNWOWWNDPROCEX call shape from original wowuserp.h. */
typedef DWORD (WINAPI *wow_window_callback)(HWND, UINT, UINT, LONG, DWORD, PVOID);
typedef struct wow_window_dispatch_target {
    DWORD procedure; /* Original encoded WNDPROC, never decoded here. */
    wow_window_callback callback;
} wow_window_dispatch_target;

/* OpenNT represents a 16-bit WNDPROC in a 32-bit USER word by setting bit
 * 31.  A selector whose own high bit is set is made unambiguous by clearing
 * its LDT bit in the encoded value; the WOW dispatcher restores it before
 * entering the guest.  These are protocol values from wuclass.c, not native
 * procedure addresses. */
#define WOW_WINDOW_PROC_TAG 0x80000000u
#define WOW_WINDOW_SELECTOR_VIRTUAL_BIT 0x00040000u

/* Live original WND fields consumed by cleanup. Stored with the window's WW
 * and callback binding, never as an independent cleanup snapshot. The USER
 * owner supplies serialized thread membership; tree links are derived only
 * during cleanup, not a persistent USER tree. Native HWND lifetime is
 * still external. Borrowers retain the containing window/class storage. */
typedef struct wow_cleanup_window {
    struct wow_cleanup_window *spwndChild, *spwndNext;
    struct wow_class_lookup_entry *pcls;
    struct wow_task_order_thread *thread;
    WNDPROC lpfnWndProc;
    BOOL serverSideProc;
} wow_cleanup_window;

/* One synchronous call view, never a guest WND, kernel pointer, persistent
 * registry or substitute for the window-owned original WW storage. The
 * caller keeps its window/WW alive across reentrant callbacks. */
typedef struct wow_window_dispatch_view {
    HWND window;
    DWORD *adwWOW;
    wow_window_callback callback;
} wow_window_dispatch_view;

LONG WINAPI DispatchClientMessage(wow_window_dispatch_view *, UINT, WPARAM, LPARAM, DWORD);

#ifdef WOW_ORIGINAL_CLIENT_DISPATCH
typedef wow_window_dispatch_view *PWND;
#define HW(pwnd) ((pwnd)->window)
#define pfnWowWndProcEx (pwnd->callback)
/* Original retail UserAssert does not evaluate its expression. The NT4
 * desktop delta is not meaningful in this finite synchronous native view. */
#define UserAssert(condition) ((void)0)
#endif
#endif
