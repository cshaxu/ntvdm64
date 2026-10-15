#ifndef WOW_USER_TASK_LIFECYCLE_H
#define WOW_USER_TASK_LIFECYCLE_H

#include "wow_input_group_bindings.h"
#include "wow_task_init_bindings.h"
#include "wow_cleanup_bindings.h"
#include "wow_user_message_bridge.h"
#include "wow_user_runtime.h"
#include "wow_class_client_bindings.h"
#include "common/protocol/window_style.h"

struct wow_user_task_lifecycle_thread;

/* The two callbacks are supplied verbatim by WOW32 at the original
 * UserRegisterWowHandlers boundary.  They are retained here only so a later
 * InitTask can bind the recovered original class client to the current task;
 * this carrier does not choose a version rule or replace the WOW callback. */
typedef struct wow_user_task_lifecycle_callbacks {
    DWORD (WINAPI *get_version)(HANDLE);
    wow_window_callback callback;
} wow_user_task_lifecycle_callbacks;

/* ADAPTER-WOW-051: one provider-local carrier for the selected original
 * taskman/queue/exitwin bodies.  It is not an NT4 USER process, a guest task
 * table or a second scheduler: original TDB ordering remains in taskman.c;
 * native queue/event ownership is the only supplied boundary. */
typedef struct wow_user_task_lifecycle {
    wow_user_runtime *runtime;
    wow_task_order_shared shared;
    wow_task_order_state state;
    wow_task_order_process process;
    wow_input_desktop desktop;
    wow_class_lookup_context classes;
    wow_user_object_table *objects;
    wow_cleanup_shared cleanup_shared;
    wow_cleanup_desktop_info cleanup_desktop;
    wow_cleanup_context cleanup;
    wow_user_task_lifecycle_callbacks callbacks;
    struct wow_user_task_lifecycle_thread *threads;
    BOOL initialized;
    BOOL registered;
} wow_user_task_lifecycle;

BOOL WINAPI wow_user_task_lifecycle_initialize(wow_user_task_lifecycle *,
    wow_user_runtime *, const wow_user_task_lifecycle_callbacks *);
BOOL WINAPI wow_user_task_lifecycle_register(wow_user_task_lifecycle *,
    wow_task_end_callback, HANDLE);
BOOL WINAPI wow_user_task_lifecycle_init(wow_user_task_lifecycle *, UINT,
    LPCSTR, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, WORD);
/* Called before original InitTask can publish a task. Direct requests pass
 * their copied value and parent=0; local guest children name their original
 * TDB_Parent. Inheritance never selects an arbitrary other WOW task. */
BOOL WINAPI wow_user_task_lifecycle_init_style(wow_user_task_lifecycle *, UINT,
    LPCSTR, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, WORD, DWORD, WORD);
BOOL WINAPI wow_user_task_lifecycle_style(wow_user_task_lifecycle *, DWORD *);
/* Retry only the launcher notification after original InitTask committed.
 * The callback runs outside the USER lock; task/request identity must match. */
DWORD WINAPI wow_user_task_lifecycle_notify_start(wow_user_task_lifecycle *,
    DWORD, DWORD, DWORD (WINAPI *)(ULONG));
BOOL WINAPI wow_user_task_lifecycle_yield(wow_user_task_lifecycle *);
VOID WINAPI wow_user_task_lifecycle_directed_yield(wow_user_task_lifecycle *,
    DWORD);
/* Preserves xxxSleepTask's result, which is FALSE on its normal task-resumed
 * path; callers must not reinterpret the return value as a generic success
 * flag.  Invalid carrier entry remains a FALSE return with ERROR_INVALID_STATE. */
BOOL WINAPI wow_user_task_lifecycle_wait(wow_user_task_lifecycle *, HANDLE);
BOOL WINAPI wow_user_task_lifecycle_message(wow_user_task_lifecycle *, LPMSG,
    HWND, UINT, UINT, UINT, BOOL);
BOOL WINAPI wow_user_task_lifecycle_wait_message(wow_user_task_lifecycle *);
DWORD WINAPI wow_user_msg_wait(DWORD, const HANDLE *, BOOL, DWORD, DWORD);
/* Borrowed call edge only. Native USER owns the operation and its result;
 * original taskman owns the two registered tasks. No pending-message queue. */
typedef struct wow_user_native_call {
    wow_user_runtime_thread *binding;
    wow_task_order_message message;
    wow_task_order_message *previous;
    DWORD receiver_id;
    BOOL held;
    HWND returned_window;
    HWND returned_payload[4];
    ULONG returned_count;
    HMENU returned_menu;
} wow_user_native_call;
BOOL WINAPI wow_user_native_call_begin(wow_user_native_call *, HWND);
BOOL WINAPI wow_user_native_call_end(wow_user_native_call *);
void WINAPI wow_user_native_call_result(wow_user_native_call *, UINT, WPARAM, LPARAM, LRESULT);
__declspec(noreturn) void WINAPI wow_user_native_call_failed(const char *, DWORD);
/* Original pfnWOWCleanup resource operation, including task==0 module calls.
 * The task carrier survives until the runtime's native-thread unbind edge. */
BOOL WINAPI wow_user_task_lifecycle_exit(wow_user_task_lifecycle *, HANDLE,
    DWORD, PNEMODULESEG, DWORD);
/* Quiesced process release, after task/object/application-class retirement.
 * Refuses live ownership instead of discarding it. Native registrations are
 * borrowed and survive release of their project metadata. */
BOOL WINAPI wow_user_task_lifecycle_dispose(wow_user_task_lifecycle *);

#endif
