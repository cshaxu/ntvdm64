#ifndef WOW_TASK_CLEANUP_ENTRY_H
#define WOW_TASK_CLEANUP_ENTRY_H
#include <windows.h>
/* Bind once before original DLL attach; provider and original TD live in C2. */
typedef struct wow_task_cleanup_provider {
    BOOL (WINAPI *uninitialized)(DWORD);
    VOID (WINAPI *retire_async)(void);
    VOID (WINAPI *free_task)(PVOID);
} wow_task_cleanup_provider;
BOOL WINAPI wow_task_cleanup_bind(const wow_task_cleanup_provider *);
/* Before guest GlobalFreeAll, or original remote/startup resource cleanup.
 * Does not free the task carrier or unbind the session/CPU. */
BOOL WINAPI wow_user_task_prepare_exit(void);
VOID WINAPI wow_user_task_retire_async(void);
/* Exact current original TD, still linked but never admitted by InitTask. */
BOOL WINAPI wow_user_task_cleanup_uninitialized(DWORD task_id);
#endif
