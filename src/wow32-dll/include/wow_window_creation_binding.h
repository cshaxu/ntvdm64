#ifndef WOW_WINDOW_CREATION_BINDING_H
#define WOW_WINDOW_CREATION_BINDING_H
#include "wow_window_words_binding.h"
#include "wow_task_order_bindings.h"
#include "wow_dialog_native_bindings.h"

/* Native creation boundary after original client policy/conversion. words
 * is a complete WW, NOT the original 12-byte lpWOW input. The composition
 * owner initializes that view and serializes class metadata access. The
 * task's independent native terminal observer retires WW on message return;
 * the actual WND procedure may be immutable guest or native. No GUI lock may
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
/* Original dialog producer's NtUserCreateWindowEx shape. Strings are already
 * captured, class lookup uses the existing class owner, and explicit version
 * and creator flags are preserved through the existing enrollment scope.
 * The modern native service receives no original WOW-word pointer. */
HWND WINAPI wow_user_create_dialog_window_ex(DWORD, PLARGE_STRING,
    PLARGE_STRING, DWORD, int, int, int, int, HWND, HMENU, HANDLE, LPVOID,
    DWORD, LPDWORD);
/* Only original comboini.c's two internal _CreateWindowEx calls: atom class,
 * null title/WOW words, zero or ANSI creator flags, tool/client-edge styles.
 * Original producer supplies all parameters; no child-ID/class inference. */
HWND WINAPI wow_control_create_window_ex(DWORD, LPCTSTR, LPCTSTR, DWORD, int,
    int, int, int, HWND, HMENU, HANDLE, LPVOID, DWORD, LPDWORD);
/* Per-window creation state, retained with WW rather than an explicit
 * CreateWindow call. Native internal creation has no such call scope.
 * The caller borrows the window/class through resource-loading reentry. */
typedef struct wow_window_creation_menu_state {
    BOOL checked;
    BOOL attached;
    HMENU input_menu;
    DWORD failure;
} wow_window_creation_menu_state;
/* Missing class-resource boundary after CBT, before guest WM_NCCREATE.
 * Keep resource conversion in the original menu client. */
BOOL WINAPI wow_window_creation_menu(HWND, CREATESTRUCTA *,
    wow_class_lookup_entry *, wow_window_creation_menu_state *);
/* One callback-local enrollment transaction. A missing borrow means that
 * another creation scope already owns the HWND, or the class is outside the
 * admitted closure. Leaving must never retire that other scope's storage.
 * These operations do not advance the native hook chain. They acquire the
 * current runtime's USER domain for metadata changes when needed, restoring
 * the caller's entry lock state; no guest execution owner is acquired. */
typedef struct wow_window_creation_enrollment {
    HWND window;
    wow_window_words_binding *borrow;
    WNDPROC previous_native;
    WNDPROC installed_native;
} wow_window_creation_enrollment;
BOOL WINAPI wow_window_creation_enroll_internal(wow_task_order_thread *, HWND,
    const CREATESTRUCTA *, wow_window_creation_enrollment *);
void WINAPI wow_window_creation_finish(wow_window_creation_enrollment *, BOOL);
/* Installed only on an initialized WOW task's native thread. Runtime TLS
 * supplies that task; no process-wide hook or extra task registry. */
LRESULT CALLBACK wow_window_creation_observer(int, WPARAM, LPARAM);
/* Forward a scoped CBT notification. Known application classes inherit their
 * logical class target; borrowed native controls require an enrolled parent.
 * Unknown classes remain outside this boundary. */
LRESULT WINAPI wow_window_creation_forward_internal(wow_task_order_thread *,
    int, WPARAM, LPARAM);
#endif
