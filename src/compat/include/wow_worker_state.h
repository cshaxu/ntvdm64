#ifndef COMPAT_WOW_WORKER_STATE_H
#define COMPAT_WOW_WORKER_STATE_H
#include <windows.h>
/* Queries the current host context through its one NTVDM-owned TLS binding. */
BOOL WINAPI wow_user_worker_active(void);
#endif
