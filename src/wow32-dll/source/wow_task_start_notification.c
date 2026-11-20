#include "wow_task_start_notification.h"
#include "wow_user_task_lifecycle.h"

DWORD WINAPI wow_user_task_lifecycle_notify_start(wow_user_task_lifecycle *owner,
    DWORD task_id, DWORD shared_id, DWORD (WINAPI *notify)(ULONG))
{
    DWORD error;
    char attempt;
    if (!notify) return ERROR_INVALID_STATE;
    error = wow_user_task_start_acquire(owner, task_id, shared_id, &attempt);
    if (error) return error;
    error = ERROR_OPERATION_ABORTED;
    __try {
        error = notify(shared_id);
    } __finally {
        error = wow_user_task_start_complete(owner, &attempt, error);
    }
    return error;
}
