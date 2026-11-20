#ifndef WOW_TASK_START_NOTIFICATION_H
#define WOW_TASK_START_NOTIFICATION_H
#include <windows.h>
struct wow_user_task_lifecycle;
/* Product notification and retry orchestration remain outside the USER owner. */
DWORD WINAPI wow_user_task_lifecycle_notify_start(struct wow_user_task_lifecycle *,
    DWORD, DWORD, DWORD (WINAPI *)(ULONG));
#endif
