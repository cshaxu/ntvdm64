#ifndef WOW_TASK_CLEANUP_ENTRY_H
#define WOW_TASK_CLEANUP_ENTRY_H
#include <windows.h>
/* Before guest GlobalFreeAll, or original remote/startup resource cleanup.
 * Does not free the task carrier or unbind the session/CPU. */
BOOL WINAPI wow_user_task_prepare_exit(void);
/* Exact current original TD, still linked but never admitted by InitTask. */
BOOL WINAPI wow_user_task_cleanup_uninitialized(DWORD task_id);
#endif
