#ifndef WOW_USER_TASK_LIFECYCLE_H
#define WOW_USER_TASK_LIFECYCLE_H

#include "wow_input_group_bindings.h"
#include "wow_input_group_lifetime.h"
#include "wow_task_init_bindings.h"
#include "wow_cleanup_bindings.h"
#include "wow_user_message_bridge.h"
#include "compat/include/wow_user_runtime.h"
#include "wow_user_native_call.h"
#include "wow_task_cleanup_entry.h"
#include "wow_class_client_bindings.h"

struct wow_user_task_lifecycle_thread;

/* These callbacks come from the original UserRegisterWowHandlers boundary.
 * The module adapter removes the original WOW procedure tag before calling
 * WOW32. They are retained here only so a later
 * InitTask can bind the recovered original class client to the current task;
 * this carrier does not choose a version rule or replace the WOW callback. */
typedef struct wow_user_task_lifecycle_callbacks {
    DWORD (WINAPI *get_version)(HANDLE);
    wow_window_callback callback;
    WORD (WINAPI *procedure_module)(DWORD);
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
    wow_input_group_owner input;
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
BOOL WINAPI wow_user_task_lifecycle_init(wow_user_task_lifecycle *, UINT, DWORD, DWORD,
    LPCSTR, LPCSTR, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD);
/* Complete the copied USER view after original InitTask has created its
 * task carrier but before W32Thread can enter the 16-bit application.  This
 * reuses the ordinary return publisher; it creates no scheduler or object
 * ownership path. */
BOOL WINAPI wow_user_task_lifecycle_publish_initial_view(
    wow_user_task_lifecycle *);
/* Begin/end a caller-owned post-initialization operation. Acquire returns
 * with the runtime unlocked. Complete must run in finally after success;
 * it re-resolves the task and validates the live frame token. No external
 * operation or callback executes inside this lifecycle implementation. */
DWORD WINAPI wow_user_task_start_acquire(wow_user_task_lifecycle *,
    DWORD, DWORD, const void *);
DWORD WINAPI wow_user_task_start_complete(wow_user_task_lifecycle *,
    const void *, DWORD);
BOOL WINAPI wow_user_task_lifecycle_yield(wow_user_task_lifecycle *);
VOID WINAPI wow_user_task_lifecycle_directed_yield(wow_user_task_lifecycle *,
    DWORD);
/* Preserves xxxSleepTask's result, which is FALSE on its normal task-resumed
 * path; callers must not reinterpret the return value as a generic success
 * flag.  Invalid carrier entry remains a FALSE return with ERROR_INVALID_STATE. */
BOOL WINAPI wow_user_task_lifecycle_wait(wow_user_task_lifecycle *, HANDLE);
BOOL WINAPI wow_user_task_lifecycle_message(wow_user_task_lifecycle *, LPMSG,
    HWND, UINT, UINT, UINT, BOOL);
BOOL WINAPI wow_user_task_lifecycle_messageW(wow_user_task_lifecycle *, LPMSG,
    HWND, UINT, UINT, UINT, BOOL);
BOOL WINAPI wow_user_task_lifecycle_wait_message(wow_user_task_lifecycle *);
DWORD WINAPI wow_user_msg_wait(DWORD, const HANDLE *, BOOL, DWORD, DWORD);
/* Original pfnWOWCleanup resource operation, including task==0 module calls.
 * The task carrier survives until the runtime's native-thread unbind edge. */
BOOL WINAPI wow_user_task_lifecycle_exit(wow_user_task_lifecycle *, HANDLE,
    DWORD);
/* Quiesced process release, after task/object/application-class retirement.
 * Refuses live ownership instead of discarding it. Native registrations are
 * borrowed and survive release of their project metadata. */
BOOL WINAPI wow_user_task_lifecycle_dispose(wow_user_task_lifecycle *);
/* Retire while the binding is current; release it only after success. */
BOOL WINAPI wow_user_task_lifecycle_unbind(wow_user_runtime_thread *);

#endif
