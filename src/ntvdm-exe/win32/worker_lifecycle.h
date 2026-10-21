#ifndef NTVDM_WORKER_LIFECYCLE_H
#define NTVDM_WORKER_LIFECYCLE_H
#include <windows.h>
#include "ntvdm-exe/session/session.h"
DWORD ntvdm_worker_lifecycle_begin(session *owner,BOOL character_io);
void ntvdm_worker_lifecycle_end(void);
#endif
