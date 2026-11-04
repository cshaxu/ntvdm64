#ifndef COMMON_WOW_WORKER_H
#define COMMON_WOW_WORKER_H
#include <windows.h>
/* Queries the sole worker-owned session; it exposes no session storage. */
BOOL WINAPI wow_user_worker_active(void);
#endif
