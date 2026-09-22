#ifndef WOW_USER_TASK_LIFECYCLE_H
#define WOW_USER_TASK_LIFECYCLE_H

#include "wow_input_group_bindings.h"
#include "wow_task_init_bindings.h"
#include "wow_cleanup_bindings.h"
#include "wow_user_message_bridge.h"
#include "wow_user_runtime.h"
#include "wow_class_client_bindings.h"

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
BOOL WINAPI wow_user_task_lifecycle_yield(wow_user_task_lifecycle *);
VOID WINAPI wow_user_task_lifecycle_directed_yield(wow_user_task_lifecycle *,
    DWORD);
/* Preserves xxxSleepTask's result, which is FALSE on its normal task-resumed
 * path; callers must not reinterpret the return value as a generic success
 * flag.  Invalid carrier entry remains a FALSE return with ERROR_INVALID_STATE. */
BOOL WINAPI wow_user_task_lifecycle_wait(wow_user_task_lifecycle *, HANDLE);
BOOL WINAPI wow_user_task_lifecycle_wow_cleanup(wow_user_task_lifecycle *,
    HANDLE, DWORD, PNEMODULESEG, DWORD);
BOOL WINAPI wow_user_task_lifecycle_cleanup(wow_user_task_lifecycle *,
    DWORD);
void WINAPI wow_user_task_lifecycle_dispose(wow_user_task_lifecycle *);

#endif
