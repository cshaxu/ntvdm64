#ifndef WOW_USER_RUNTIME_H
#define WOW_USER_RUNTIME_H

#include <windows.h>

struct wow_task_order_thread;
struct wow_class_client_context;

/* ADAPTER-WOW-049: worker-local carrier for the original USER process and
 * current-thread invariants. This is neither an NT4 PROCESSINFO/THREADINFO
 * clone nor a guest-visible shared USER heap. The caller owns storage and
 * initializes it to zero before first use. One worker owns one domain. */
typedef struct wow_user_runtime {
    INIT_ONCE initialization;
    SRWLOCK exclusive;
    DWORD process_id;
    volatile DWORD owner_thread_id;
    /* One provider-local W1 lifecycle carrier is attached to this worker
     * domain.  It is opaque here: the runtime neither allocates tasks nor
     * chooses scheduling policy. */
    PVOID lifecycle;
    BOOL (WINAPI *retire_thread)(PVOID lifecycle);
} wow_user_runtime;

typedef struct wow_user_runtime_thread {
    wow_user_runtime *runtime;
    struct wow_task_order_thread *thread;
    struct wow_class_client_context *classes;
    BOOL exclusive_held;
} wow_user_runtime_thread;

#define WOW_USER_RUNTIME_INITIALIZER { INIT_ONCE_STATIC_INIT, SRWLOCK_INIT, 0, 0, NULL, NULL }

BOOL WINAPI wow_user_runtime_initialize(wow_user_runtime *);
BOOL WINAPI wow_user_runtime_bind(wow_user_runtime_thread *, wow_user_runtime *,
    struct wow_task_order_thread *, struct wow_class_client_context *);
BOOL WINAPI wow_user_runtime_unbind(wow_user_runtime_thread *);
wow_user_runtime_thread *WINAPI wow_user_runtime_current(void);
BOOL WINAPI wow_user_runtime_enter(wow_user_runtime_thread *);
BOOL WINAPI wow_user_runtime_leave(wow_user_runtime_thread *);
/* These pair around a USER-to-client callout only. A borrowed object scope
 * remains owned by its caller; holding the domain lock through a callback is
 * prohibited by the original USER contract. */
BOOL WINAPI wow_user_runtime_leave_for_callout(wow_user_runtime_thread *);
BOOL WINAPI wow_user_runtime_reenter_after_callout(wow_user_runtime_thread *);
BOOL WINAPI wow_user_runtime_set_context(wow_user_runtime_thread *,
    struct wow_task_order_thread *, struct wow_class_client_context *);

#endif
