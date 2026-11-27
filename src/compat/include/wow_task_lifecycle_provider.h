#ifndef WOW_TASK_LIFECYCLE_PROVIDER_H
#define WOW_TASK_LIFECYCLE_PROVIDER_H
#include "wow_task_init_bindings.h"

/* Borrowed image-lifetime operations. The original scheduler retains task
 * selection and destruction; the caller retains its existing lock/scope. */
typedef struct wow_task_lifecycle_provider {
    BOOL (WINAPI *sleep)(BOOL, HANDLE, wow_task_order_thread *);
    VOID (WINAPI *directed_yield)(DWORD);
    NTSTATUS (WINAPI *initialize)(UINT, DWORD, DWORD, PUNICODE_STRING, DWORD, DWORD, DWORD,
        DWORD, DWORD, DWORD, DWORD, WORD);
    BOOL (WINAPI *register_hung_handlers)(wow_task_end_callback, HANDLE,
        wow_task_order_process *);
    BOOL (WINAPI *yield)(wow_task_order_thread *);
    VOID (WINAPI *destroy)(wow_task_order_process *, wow_task_order_thread *);
} wow_task_lifecycle_provider;

BOOL WINAPI wow_task_lifecycle_bind(const wow_task_lifecycle_provider *);
#endif
