#ifndef WOW_USER_NATIVE_CALL_H
#define WOW_USER_NATIVE_CALL_H
#include "wow_user_runtime.h"
#include "wow_task_order_bindings.h"

/* Borrowed call edge only. Native USER owns the operation and its result;
 * original taskman owns the two registered tasks. No pending-message queue.
 * Keep this declaration closure independent of registration/cleanup's
 * unguarded original wowuserp.h; original thunk units already include it. */
typedef struct wow_user_native_call {
    wow_user_runtime_thread *binding;
    wow_task_execution_scope execution;
    BOOL held, execution_held;
    HWND target_window;
    HWND returned_window;
    HWND returned_payload[4];
    ULONG returned_count;
    HMENU returned_menu;
} wow_user_native_call;
BOOL WINAPI wow_user_native_call_begin(wow_user_native_call *, HWND);
/* Nonblocking synchronous native dispatch (Peek/Enum/CallNextHook).
 * Retain the inherited owner; actual cross-task callback entry can park
 * that native frame without inventing a send/reply association. */
BOOL WINAPI wow_user_native_call_begin_inline(wow_user_native_call *);
BOOL WINAPI wow_user_native_call_end(wow_user_native_call *);
/* A modeless original leaf can return a public HWND without requiring the
 * adapter's unrelated full desktop-graph projection.  This still restores
 * the exact scheduler/runtime handoff and preserves the native result. */
BOOL WINAPI wow_user_native_call_end_no_publication(wow_user_native_call *);
/* Scheduler-wait entry did not necessarily own guest execution. Restore its
 * borrowed native frame without pretending to return to guest in that case. */
BOOL WINAPI wow_user_native_call_end_wait(wow_user_native_call *);
void WINAPI wow_user_native_call_result(wow_user_native_call *, UINT, WPARAM, LPARAM, LRESULT);
#endif
